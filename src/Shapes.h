#pragma once

#include <cmath>

namespace shapes
{
struct Point
{
	float x, y;

	float DistanceTo(const Point& rhs) const
	{
		float dx = x - rhs.x;
		float dy = y - rhs.y;

		return std::sqrt(dx * dx + dy * dy);
	}
};

struct Segment
{
	Point p1, p2;
};
} // namespace shapes
