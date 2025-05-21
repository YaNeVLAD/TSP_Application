#include "../TSPManager.h"

TSPManager::TSPManager()
	: m_calculationNeeded(false)
	, m_calculationInProgress(false)
	, m_stopCalculationThread(false)
	, m_enableACOCalculation(true)
	, m_enableBruteForceCalculation(true)
	, m_calculationThread(&TSPManager::CalculationThreadFunc, this)
{
}

TSPManager::~TSPManager()
{
	m_stopCalculationThread.store(true);
	m_cv.notify_one();
}

void TSPManager::AddCity(float x, float y)
{
	m_cities.push_back({ m_cities.size(), x, y });
}
void TSPManager::RemoveLastCity()
{
	if (!m_cities.empty())
	{
		m_cities.pop_back();
	}
}

void TSPManager::UpdateCityPosition(int index, float x, float y)
{
	if (index >= 0 && index < m_cities.size())
	{
		m_cities[index].pos.x = x;
		m_cities[index].pos.y = y;
	}
}

void TSPManager::TriggerCalculation()
{
	bool needToRecalc = m_cities.size() >= 2
		&& (m_enableACOCalculation.load() || m_enableBruteForceCalculation.load());

	if (needToRecalc)
	{
		std::lock_guard<std::mutex> lock(m_mtx);
		m_pendingCities = m_cities;
		m_calculationNeeded.store(true);
		m_cv.notify_one();
	}
	else
	{
		std::lock_guard<std::mutex> lock(m_mtx);
		m_currentResults.acoTour.clear();
		m_currentResults.bruteForceTourIndices.clear();
		m_currentResults.acoCost = 0.0;
		m_currentResults.bruteForceCost = 0.0;
	}
}

TSPResults TSPManager::GetCurrentResults()
{
	std::lock_guard<std::mutex> lock(m_mtx);
	return m_currentResults;
}

Matrix<double> TSPManager::CreateDistanceMatrix(const std::vector<salesman_problem::City>& cities_ref)
{
	size_t n = cities_ref.size();
	Matrix<double> distances(n, n);
	distances.ForEach([&cities_ref](size_t i, size_t j) {
		return cities_ref[i].pos.DistanceTo(cities_ref[j].pos);
	});
	return distances;
}

void TSPManager::CalculationThreadFunc()
{
	std::random_device rd;
	std::mt19937 gen(rd());

	while (true)
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		m_cv.wait(lock, [this] { return m_calculationNeeded.load() || m_stopCalculationThread.load(); });

		if (m_stopCalculationThread.load())
		{
			break;
		}

		m_calculationInProgress.store(true);
		m_calculationNeeded.store(false);

		std::vector<salesman_problem::City> citiesToSolve = m_pendingCities;
		lock.unlock(); // Release lock during heavy computation

		TSPResults newResults;
		newResults.cities = citiesToSolve;

		// Initialize results with current values before potential recalculation
		{
			std::lock_guard<std::mutex> currentLock(m_mtx);
			newResults.acoCost = m_currentResults.acoCost;
			newResults.bruteForceCost = m_currentResults.bruteForceCost;
			newResults.acoTour = m_currentResults.acoTour;
			newResults.bruteForceTourIndices = m_currentResults.bruteForceTourIndices;
		}

		if (citiesToSolve.size() >= 2)
		{
			if (m_enableACOCalculation.load())
			{
				size_t n = citiesToSolve.size();
				auto spec = salesman_problem::ACOSpec::MMAS(n);
				salesman_problem::AntColonyOptimization aco(citiesToSolve, gen, spec);
				aco.SetCities(citiesToSolve);
				aco.Solve();
				newResults.acoTour = aco.GetBestTour();
				newResults.acoCost = aco.GetBestTourLength();
			}
			else
			{
				newResults.acoTour.clear();
			}

			if (m_enableBruteForceCalculation.load())
			{
				Matrix<double> distances = CreateDistanceMatrix(citiesToSolve);
				auto [path, cost] = salesman_problem::ShortestHamiltonianPath(distances);
				newResults.bruteForceTourIndices = path;
				newResults.bruteForceCost = cost;
			}
			else
			{
				newResults.bruteForceTourIndices.clear();
			}
		}
		else
		{
			newResults.acoTour.clear();
			newResults.bruteForceTourIndices.clear();
			newResults.acoCost = 0.0;
			newResults.bruteForceCost = 0.0;
		}

		lock.lock();
		m_currentResults = newResults;
		m_calculationInProgress.store(false);
		lock.unlock();
		m_cv.notify_one();
	}
}
