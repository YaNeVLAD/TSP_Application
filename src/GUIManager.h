#pragma once

#include <atomic>

#include "TSPManager.h"
#include "Window.h"

namespace sf
{
class RenderWindow;
class Clock;
class Event;
} // namespace sf

class GUIManager
{
public:
	GUIManager(TSPManager& manager, const Window& window);
	~GUIManager();

	void Update(sf::Clock& clock);
	void Display();
	void ProcessEvents(sf::Event& event);

	void Render();

	bool ShouldShowCities() const { return m_showCities; }
	bool ShouldShowACOTour() const { return m_showACOTour.load(); }
	bool ShouldShowBruteForceTour() const { return m_showBruteForceTour.load(); }
	bool PopRecalculationTrigger();

private:
	TSPManager& m_tspManager;
	sf::RenderWindow* m_window;

	bool m_showCities;
	std::atomic_bool m_showACOTour;
	std::atomic_bool m_showBruteForceTour;
	bool m_recalculationTriggeredByUI;
};
