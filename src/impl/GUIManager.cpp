#include "../GUIManager.h"

#include "imgui-SFML.h"
#include "imgui.h"
#include <SFML/Graphics.hpp>

GUIManager::GUIManager(TSPManager& manager, const Window& window)
	: m_tspManager(manager)
	, m_window(window.GetRaw())
	, m_showCities(true)
	, m_showACOTour(true)
	, m_showBruteForceTour(true)
	, m_recalculationTriggeredByUI(false)

{
	if (m_window)
	{
		ImGui::SFML::Init(*m_window);
	}
}

GUIManager::~GUIManager()
{
	ImGui::SFML::Shutdown();
}

void GUIManager::Update(sf::Clock& clock)
{
	ImGui::SFML::Update(*m_window, clock.restart());
}

void GUIManager::Display()
{
	ImGui::SFML::Render(*m_window);
}

void GUIManager::ProcessEvents(sf::Event& event)
{
	ImGui::SFML::ProcessEvent(event);
}

void GUIManager::Render()
{
	ImGui::Begin("TSP Controls");
	ImGui::Text("Cities: %zu", m_tspManager.GetCityCount());

	if (ImGui::Button("Add City"))
	{
		std::random_device rd_local;
		std::mt19937 gen_local(rd_local());
		std::uniform_real_distribution<float> dist(0.0, 15.0);
		m_tspManager.AddCity(dist(gen_local), dist(gen_local));
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove Last City"))
	{
		m_tspManager.RemoveLastCity();
	}

	if (ImGui::Button("Recalculate Paths"))
	{
		m_recalculationTriggeredByUI = true;
	}

	ImGui::Separator();
	ImGui::Text("Tour Costs:");
	if (m_tspManager.IsCalculationInProgress())
	{
		ImGui::Text("ACO Tour Cost: Calculating...");
		ImGui::Text("Brute Force Tour Cost: Calculating...");
	}
	else
	{
		TSPResults results = m_tspManager.GetCurrentResults();
		ImGui::Text("ACO Tour Cost: %.2f", results.acoCost);
		ImGui::Text("Brute Force Tour Cost: %.2f", results.bruteForceCost);
	}
	ImGui::Separator();
	ImGui::Checkbox("Show Cities", &m_showCities);

	bool currentShowACO = m_showACOTour.load();
	if (ImGui::Checkbox("Show ACO Tour", &currentShowACO))
	{
		m_showACOTour.store(currentShowACO);
	}
	ImGui::SameLine();
	bool currentEnableACOCalc = m_tspManager.IsACOCalculationEnabled();
	if (ImGui::Checkbox("Enable ACO Calculation", &currentEnableACOCalc))
	{
		m_tspManager.SetEnableACOCalculation(currentEnableACOCalc);
	}

	bool currentShowBruteForce = m_showBruteForceTour.load();
	if (ImGui::Checkbox("Show Brute Force Tour", &currentShowBruteForce))
	{
		m_showBruteForceTour.store(currentShowBruteForce);
	}
	ImGui::SameLine();
	bool currentEnableBruteForceCalc = m_tspManager.IsBruteForceCalculationEnabled();
	if (ImGui::Checkbox("Enable Brute Force Calculation", &currentEnableBruteForceCalc))
	{
		m_tspManager.SetEnableBruteForceCalculation(currentEnableBruteForceCalc);
	}

	ImGui::Separator();
	bool currentIsACSEnabled = m_tspManager.IsACSEnabled();
	if (ImGui::Checkbox("Use ACS in Ant colony optimization", &currentIsACSEnabled))
	{
		m_tspManager.SetIsACSEnabled(currentIsACSEnabled);
	}

	ImGui::End();
}

bool GUIManager::PopRecalculationTrigger()
{
	bool triggered = m_recalculationTriggeredByUI;
	m_recalculationTriggeredByUI = false;
	return triggered;
}
