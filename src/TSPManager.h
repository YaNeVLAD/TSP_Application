#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

#include "ACO.h"
#include "BrootForce.h"
#include "Matrix.h"

struct TSPResults
{
	std::vector<salesman_problem::City> cities;
	std::vector<salesman_problem::City> acoTour;
	std::vector<size_t> bruteForceTourIndices;
	double acoCost = 0.0;
	double bruteForceCost = 0.0;
};

class TSPManager
{
public:
	TSPManager();

	~TSPManager();

	void AddCity(float x, float y);
	void RemoveLastCity();
	void UpdateCityPosition(int index, float x, float y);

	const std::vector<salesman_problem::City>& GetCities() const { return m_cities; }
	size_t GetCityCount() const { return m_cities.size(); }

	void TriggerCalculation();
	bool IsCalculationInProgress() const { return m_calculationInProgress.load(); }
	TSPResults GetCurrentResults();

	void SetEnableACOCalculation(bool enable) { m_enableACOCalculation.store(enable); }
	void SetEnableBruteForceCalculation(bool enable) { m_enableBruteForceCalculation.store(enable); }
	bool IsACOCalculationEnabled() const { return m_enableACOCalculation.load(); }
	bool IsBruteForceCalculationEnabled() const { return m_enableBruteForceCalculation.load(); }

private:
	std::vector<salesman_problem::City> m_cities;
	std::mutex m_mtx;
	std::condition_variable m_cv;
	std::atomic_bool m_calculationNeeded;
	std::atomic_bool m_calculationInProgress;
	std::atomic_bool m_stopCalculationThread;

	std::atomic_bool m_enableACOCalculation;
	std::atomic_bool m_enableBruteForceCalculation;

	TSPResults m_currentResults;
	std::vector<salesman_problem::City> m_pendingCities;

	std::jthread m_calculationThread;

	static Matrix<double> CreateDistanceMatrix(const std::vector<salesman_problem::City>& cities_ref);

	void CalculationThreadFunc();
};
