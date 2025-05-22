#pragma once

#include <cmath>

#include "GUIManager.h"
#include "TSPManager.h"
#include "Window.h"

class TSPApp
{
public:
	TSPApp();

	void Run();

private:
	Window m_window;
	TSPManager m_tspManager;
	GUIManager m_guiManager;

	int m_selectedCityIndex;
	bool m_draggingCity;

	void onMousePressed(int button, float worldX, float worldY);

	void onMouseReleased(int button, float, float);

	void onMouseMoved(float worldX, float worldY);

	void DrawAcoTour(const TSPResults& results);

	void DrawBruteForceTour(const TSPResults& results);
};
