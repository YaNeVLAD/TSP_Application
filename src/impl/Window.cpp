#include "../Window.h"

#include <SFML/Graphics.hpp>

Window::Window(unsigned width, unsigned height, const std::string& title)
	: m_window(new sf::RenderWindow(sf::VideoMode(width, height), title))
	, m_scale(50)
	, m_offset({ 50.f, 50.f })
{
}

Window::~Window()
{
	delete m_window;
}

bool Window::IsOpen()
{
	return m_window->isOpen();
}

void Window::Close()
{
	m_window->close();
}

void Window::Clear(Color fillColor)
{
	m_window->clear(sf::Color{ fillColor.ToInt() });
}

void Window::Display()
{
	m_window->display();
}

void Window::HandleEvents(const sf::Event& event)
{
	if (event.type == sf::Event::Closed)
	{
		m_window->close();
	}
	else if (event.type == sf::Event::MouseButtonPressed)
	{
		shapes::Point clickPos = { (float)event.mouseButton.x, (float)event.mouseButton.y };
		auto [worldX, worldY] = ToWorldPos(clickPos);
		if (m_mousePressedCallback)
		{
			m_mousePressedCallback(event.mouseButton.button, worldX, worldY);
		}
	}
	else if (event.type == sf::Event::MouseButtonReleased)
	{
		shapes::Point clickPos = { (float)event.mouseButton.x, (float)event.mouseButton.y };
		auto [worldX, worldY] = ToWorldPos(clickPos);
		if (m_mouseReleasedCallback)
		{
			m_mouseReleasedCallback(event.mouseButton.button, worldX, worldY);
		}
	}
	else if (event.type == sf::Event::MouseMoved)
	{
		shapes::Point clickPos = { (float)event.mouseMove.x, (float)event.mouseMove.y };
		auto [worldX, worldY] = ToWorldPos(clickPos);
		if (m_mouseMovedCallback)
		{
			m_mouseMovedCallback(worldX, worldY);
		}
	}
}

void Window::SetMouseButtonPressedCallback(MouseButtonCallback callback)
{
	m_mousePressedCallback = std::move(callback);
}

void Window::SetMouseButtonReleasedCallback(MouseButtonCallback callback)
{
	m_mouseReleasedCallback = std::move(callback);
}

void Window::SetMouseMovedCallback(MouseMoveCallback callback)
{
	m_mouseMovedCallback = std::move(callback);
}

void Window::DrawPoint(shapes::Point p, Color color, float radius) const
{
	auto c = sf::Color(color.ToInt());
	sf::CircleShape shape(radius);
	shape.setFillColor(c);

	auto [x, y] = ApplyOffset(p);

	shape.setOrigin(radius, radius);
	shape.setPosition(x, y);
	m_window->draw(shape);
}

void Window::DrawLine(shapes::Segment seg, Color color) const
{
	auto c = sf::Color(color.ToInt());
	auto [ax, ay] = ApplyOffset(seg.p1);
	auto [bx, by] = ApplyOffset(seg.p2);

	sf::Vertex line[] = {
		sf::Vertex(sf::Vector2f(ax, ay), c),
		sf::Vertex(sf::Vector2f(bx, by), c),
	};

	m_window->draw(line, 2, sf::Lines);
}

shapes::Point Window::ToWorldPos(shapes::Point p)
{
	sf::Vector2f mouseScreenPos = m_window->mapPixelToCoords(sf::Vector2i((int)p.x, (int)p.y));
	float worldX = (mouseScreenPos.x - m_offset.first) / m_scale;
	float worldY = (mouseScreenPos.y - m_offset.second) / m_scale;
	return { worldX, worldY };
}

shapes::Point Window::ApplyOffset(shapes::Point p) const
{
	auto ox = p.x * m_scale + m_offset.first;
	auto oy = p.y * m_scale + m_offset.second;
	return { ox, oy };
}
