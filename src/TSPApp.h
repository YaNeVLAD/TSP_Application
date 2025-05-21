#pragma once

#include <cmath>

#include "TSPManager.h"
#include "UIManager.h"
#include "Window.h"

class TSPApp
{
public:
	TSPApp();

	void Run();

private:
	Window m_window;
	TSPManager m_tspManager;
	UIManager m_uiManager;

	int m_selectedCityIndex;
	bool m_draggingCity;

	void onMousePressed(int button, float worldX, float worldY);

	void onMouseReleased(int button, float, float);

	void onMouseMoved(float worldX, float worldY);
};
