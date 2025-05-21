#pragma once

#include <algorithm>
#include <iostream>

#include "Matrix.h"

namespace salesman_problem
{
template <typename T>
T CalculatePathLength(const Matrix<T>& matrix, const std::vector<size_t>& path)
{
	if (path.empty())
		return 0;

	T len = 0;
	for (size_t i = 0; i < path.size() - 1; ++i)
	{
		size_t u = path[i];
		size_t v = path[i + 1];

		len += matrix(u, v);
	}

	return len;
}

template <typename T>
std::pair<std::vector<size_t>, T> ShortestHamiltonianPath(const Matrix<T>& matrix)
{
	std::vector<size_t> path(matrix.GetSize());
	for (size_t i = 0; i < path.size(); ++i)
	{
		path[i] = i;
	}

	T bestLen = std::numeric_limits<T>::max();
	std::vector<size_t> bestCycle;
	std::sort(path.begin(), path.end());

	do
	{
		std::vector<size_t> cycle(path);
		cycle.push_back(cycle.front());

		auto len = CalculatePathLength(matrix, cycle);
		if (len != 0 && len < bestLen)
		{
			bestCycle = cycle;
			bestLen = len;
		}
	} while (std::next_permutation(path.begin(), path.end()));

	return { bestCycle, bestLen };
}
} // namespace salesman_problem
