#pragma once

#include <functional>
#include <string>

#include "Color.h"
#include "Shapes.h"

namespace sf
{
class RenderWindow;
class Event;
} // namespace sf

class Window
{
	using MouseButtonCallback = std::function<void(int button, float x, float y)>;
	using MouseMoveCallback = std::function<void(float x, float y)>;

public:
	Window(unsigned width, unsigned height, const std::string& title);

	~Window();

	bool IsOpen();
	void Close();
	void Clear(Color fillColor);
	void Display();

	void HandleEvents(const sf::Event& event);

	void SetMouseButtonPressedCallback(MouseButtonCallback callback);
	void SetMouseButtonReleasedCallback(MouseButtonCallback callback);
	void SetMouseMovedCallback(MouseMoveCallback callback);

	void DrawPoint(shapes::Point p, Color color, float radius = 5.0f) const;
	void DrawLine(shapes::Segment seg, Color color) const;

	float GetScale() const { return m_scale; }

	sf::RenderWindow* const GetRaw() const { return m_window; }

private:
	sf::RenderWindow* m_window;
	float m_scale;
	std::pair<float, float> m_offset;

	MouseButtonCallback m_mousePressedCallback;
	MouseButtonCallback m_mouseReleasedCallback;
	MouseMoveCallback m_mouseMovedCallback;

	shapes::Point ToWorldPos(shapes::Point p);
	shapes::Point ApplyOffset(shapes::Point p) const;
};
