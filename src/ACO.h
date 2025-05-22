#ifndef ACO_H
#define ACO_H

#include <random>
#include <vector>

#include "Matrix.h"
#include "Shapes.h"

namespace salesman_problem
{
struct ACOSpec
{
	size_t antsCount;
	double alpha;
	double beta;
	double rho;
	double Q;
	size_t maxIterations;
	double initialPheromone;

	static ACOSpec ACS()
	{
		return ACOSpec{
			.antsCount = 10,
			.alpha = 1,
			.beta = 2,
			.rho = 0.1,
			.Q = 0.9,
			.maxIterations = 100,
			.initialPheromone = 1.0
		};
	}

	static ACOSpec MMAS(size_t n)
	{
		return ACOSpec{
			.antsCount = n,
			.alpha = 1,
			.beta = 2,
			.rho = 0.02,
			.Q = 0.0,
			.maxIterations = 100,
			.initialPheromone = 1.0
		};
	}
};

struct City
{
	size_t id;
	shapes::Point pos;
};

class AntColonyOptimization
{
	using MatrixD = Matrix<double>;

public:
	AntColonyOptimization(
		const std::vector<City>& cities,
		std::mt19937& rd,
		ACOSpec spec = ACOSpec::ACS());

	AntColonyOptimization(
		size_t cityCount,
		std::mt19937& rd,
		ACOSpec spec = ACOSpec::ACS());

	void SetCities(const std::vector<City>& cities);

	void Solve();

	std::vector<City> GetBestTour() const;

	double GetBestTourLength() const;

private:
	size_t m_cityCount;
	size_t m_antsCount;
	double m_alpha;
	double m_beta;
	double m_rho;
	double m_Q;
	size_t m_maxIterations;
	double m_initialPheromon;

	double m_bestTourLen;

	std::vector<City> m_cities;
	std::vector<size_t> m_bestTour;

	MatrixD m_distances;
	MatrixD m_pheromones;

	std::mt19937& m_random;

	std::vector<size_t> BuildTour();

	size_t SelectNextCity(size_t currID, const std::vector<bool> visited);

	double CalculateTourLength(const std::vector<size_t>& tour);

	void UpdatePheromones(const Matrix<size_t>& antTours, const std::vector<double>& antTourLengths);

	void EvaporatePheromones();
};
} // namespace salesman_problem

#endif // ACO_H
