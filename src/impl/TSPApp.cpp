#include "../TSPApp.h"

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

TSPApp::TSPApp()
	: m_window(1200, 800, "Interactive TSP Visualization")
	, m_guiManager(m_tspManager, m_window)
	, m_selectedCityIndex(-1)
	, m_draggingCity(false)
{
	m_window.SetMouseButtonPressedCallback([this](int button, float x, float y) {
		if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && !ImGui::IsAnyItemHovered())
		{
			onMousePressed(button, x, y);
		}
	});
	m_window.SetMouseButtonReleasedCallback([this](int button, float x, float y) {
		if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && !ImGui::IsAnyItemHovered())
		{
			onMouseReleased(button, x, y);
		}
	});
	m_window.SetMouseMovedCallback([this](float x, float y) {
		if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && !ImGui::IsAnyItemHovered())
		{
			onMouseMoved(x, y);
		}
	});

	m_tspManager.AddCity(5.0, 5.0);
	m_tspManager.TriggerCalculation();
}

void TSPApp::Run()
{
	sf::Clock clock;
	while (m_window.IsOpen())
	{
		sf::Event event;
		while (m_window.GetRaw()->pollEvent(event))
		{
			// Let ImGui handle events first
			m_guiManager.ProcessEvents(event);
			m_window.HandleEvents(event);
		}
		m_guiManager.Update(clock);

		if (m_guiManager.PopRecalculationTrigger())
		{
			m_tspManager.TriggerCalculation();
		}

		m_guiManager.Render();

		m_window.Clear(Color(18, 33, 43));

		const auto& cities = m_tspManager.GetCities();
		if (m_guiManager.ShouldShowCities())
		{
			for (size_t i = 0; i < cities.size(); ++i)
			{
				auto color = i == m_selectedCityIndex ? Color::Yellow : Color::White;

				m_window.DrawPoint(cities[i].pos, color);
			}
		}

		TSPResults currentResults = m_tspManager.GetCurrentResults();

		DrawAcoTour(currentResults);

		DrawBruteForceTour(currentResults);

		m_guiManager.Display();
		m_window.Display();
	}
}

void TSPApp::onMousePressed(int button, float worldX, float worldY)
{
	if (button == 0)
	{
		m_selectedCityIndex = -1;
		const auto& cities = m_tspManager.GetCities();
		for (size_t i = 0; i < cities.size(); ++i)
		{
			float dx = cities[i].pos.x - worldX;
			float dy = cities[i].pos.y - worldY;
			if (std::sqrt(dx * dx + dy * dy) < 10.0f * (1.f / m_window.GetScale()))
			{
				m_selectedCityIndex = i;
				m_draggingCity = true;
				break;
			}
		}
	}
}

void TSPApp::onMouseReleased(int button, float, float)
{
	if (button == 0)
	{
		m_draggingCity = false;
		m_selectedCityIndex = -1;
	}
}

void TSPApp::onMouseMoved(float worldX, float worldY)
{
	if (m_draggingCity && m_selectedCityIndex != -1)
	{
		m_tspManager.UpdateCityPosition(m_selectedCityIndex, worldX, worldY);
	}
}

void TSPApp::DrawAcoTour(const TSPResults& results)
{
	if (m_guiManager.ShouldShowACOTour())
	{
		for (size_t i = 0; i < results.acoTour.size(); ++i)
		{
			const auto& p1 = results.acoTour[i].pos;
			const auto& p2 = results.acoTour[(i + 1) % results.acoTour.size()].pos;
			m_window.DrawLine({ p1, p2 }, Color::Green);
		}
	}
}

void TSPApp::DrawBruteForceTour(const TSPResults& results)
{
	if (m_guiManager.ShouldShowBruteForceTour())
	{
		for (size_t i = 0; i < results.bruteForceTourIndices.size(); ++i)
		{
			const auto& p1 = results.cities[results.bruteForceTourIndices[i]].pos;
			const auto& p2 = results.cities[results.bruteForceTourIndices[(i + 1) % results.bruteForceTourIndices.size()]].pos;
			m_window.DrawLine({ p1, p2 }, Color::Red);
		}
	}
}
