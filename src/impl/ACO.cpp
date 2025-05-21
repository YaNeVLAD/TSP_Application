#include "../ACO.h"

using namespace salesman_problem;

AntColonyOptimization::AntColonyOptimization(const std::vector<City>& cities, std::mt19937& rd, ACOSpec spec)
	: AntColonyOptimization(cities.size(), rd, spec)
{
	SetCities(cities);
}

AntColonyOptimization::AntColonyOptimization(size_t cityCount, std::mt19937& randomDevice, ACOSpec spec)
	: m_cityCount(cityCount)
	, m_antsCount(spec.antsCount)
	, m_alpha(spec.alpha)
	, m_beta(spec.beta)
	, m_rho(spec.rho)
	, m_Q(spec.Q)
	, m_maxIterations(spec.maxIterations)
	, m_initialPheromon(spec.initialPheromone)
	, m_bestTourLen(std::numeric_limits<double>::max())
	, m_random(randomDevice)
{
}

void AntColonyOptimization::SetCities(const std::vector<City>& cities)
{
	size_t n = cities.size();
	m_cities = cities;
	m_cityCount = n;

	MatrixD distances(n, n);
	distances.ForEach([this, cities](size_t i, size_t j) {
		return cities[i].pos.DistanceTo(cities[j].pos);
	});
	m_distances = std::move(distances);

	MatrixD pheromones(n, n);
	pheromones.ForEach([this](size_t, size_t) {
		return m_initialPheromon;
	});
	m_pheromones = std::move(pheromones);
}

void AntColonyOptimization::Solve()
{
	for (size_t iter = 0; iter < m_maxIterations; ++iter)
	{
		Matrix<size_t> antTours(m_antsCount, m_antsCount);
		std::vector<double> antTourLengths(m_antsCount);

		for (size_t k = 0; k < m_antsCount; ++k)
		{
			antTours[k] = BuildTour();
			antTourLengths[k] = CalculateTourLength(antTours[k]);

			if (antTourLengths[k] < m_bestTourLen)
			{
				m_bestTourLen = antTourLengths[k];
				m_bestTour = antTours[k];
			}
		}

		UpdatePheromones(antTours, antTourLengths);

		EvaporatePheromones();
	}
}

std::vector<City> AntColonyOptimization::GetBestTour() const
{
	std::vector<City> result;
	result.reserve(m_cityCount);

	for (size_t i = 0; i < m_cityCount; ++i)
	{
		result.push_back(m_cities[m_bestTour[i]]);
	}
	result.push_back(m_cities[m_bestTour.front()]);

	return result;
}

double AntColonyOptimization::GetBestTourLength() const
{
	return m_bestTourLen;
}

std::vector<size_t> AntColonyOptimization::BuildTour()
{
	std::vector<size_t> tour;
	std::vector<bool> visited(m_cityCount, false);

	std::uniform_int_distribution<size_t> dist(0, m_cityCount - 1);
	size_t currentCity = dist(m_random);
	tour.push_back(currentCity);
	visited[currentCity] = true;

	for (size_t i = 1; i < m_cityCount; ++i)
	{
		size_t nextCity = SelectNextCity(currentCity, visited);
		tour.push_back(nextCity);
		visited[nextCity] = true;
		currentCity = nextCity;
	}

	return tour;
}

size_t AntColonyOptimization::SelectNextCity(size_t currID, const std::vector<bool> visited)
{
	std::vector<double> probabilities(m_cityCount, 0.0);
	double probabilitiesSum = 0.0;

	for (size_t city = 0; city < m_cityCount; ++city)
	{
		if (visited[city])
		{
			continue;
		}

		double eta = m_distances(currID, city) == 0
			? std::numeric_limits<double>::infinity()
			: 1.0;

		double tau = std::max(m_pheromones(currID, city), std::numeric_limits<double>::epsilon());

		probabilities[city] = std::pow(tau, m_alpha) * std::pow(eta, m_beta);
		probabilitiesSum += probabilities[city];
	}

	std::uniform_real_distribution<double> dist(0.0, probabilitiesSum);
	double randomVal = dist(m_random);
	double cumProb = 0.0;

	for (size_t city = 0; city < m_cityCount; ++city)
	{
		if (visited[city])
		{
			continue;
		}

		cumProb += probabilities[city];
		if (randomVal <= cumProb)
		{
			return city;
		}
	}

	// WTF
	throw std::runtime_error("Failed to find city to move from city " + std::to_string(currID));
}

double AntColonyOptimization::CalculateTourLength(const std::vector<size_t>& tour)
{
	double len = 0.0;
	for (size_t i = 0; i < tour.size() - 1; ++i)
	{
		len += m_distances(tour[i], tour[i + 1]);
	}
	len += m_distances(tour.back(), tour.front());

	return len;
}

void AntColonyOptimization::UpdatePheromones(const Matrix<size_t>& antTours, const std::vector<double>& antTourLengths)
{
	for (size_t k = 0; k < m_antsCount; ++k)
	{
		double dPheromone = m_Q / antTourLengths[k];
		const auto& tour = antTours[k];
		for (size_t i = 0; i < tour.size() - 1; ++i)
		{
			size_t city1 = tour[i];
			size_t city2 = tour[i + 1];

			m_pheromones(city1, city2) += dPheromone;
			m_pheromones(city2, city1) += dPheromone;
		}

		m_pheromones(tour.back(), tour.front()) += dPheromone;
		m_pheromones(tour.front(), tour.back()) += dPheromone;
	}
}

void AntColonyOptimization::EvaporatePheromones()
{
	m_pheromones.ForEach([this](size_t, size_t) {
		return 1.0 - m_rho;
	});
}
