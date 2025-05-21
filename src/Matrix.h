#pragma once

#include <stdexcept>
#include <vector>

template <typename T>
class Matrix
{
	static_assert(std::is_arithmetic_v<T>, "T must be arithmetic");

public:
	Matrix()
		: Matrix(0, 0)
	{
	}

	Matrix(size_t rows, size_t columns)
		: m_rows(rows)
		, m_cols(columns)
		, m_data(rows, std::vector<T>(columns))
	{
	}

	T& operator()(size_t row, size_t col)
	{
		if (row >= m_rows || col >= m_cols)
		{
			throw std::out_of_range("Matrix index out of bounds");
		}

		return m_data[row][col];
	}

	const T& operator()(size_t row, size_t col) const
	{
		return const_cast<Matrix&>(*this)(row, col);
	}

	std::vector<T>& operator[](size_t row)
	{
		if (row >= m_rows)
		{
			throw std::out_of_range("Matrix index out of bounds");
		}
		return m_data[row];
	}

	template <class Fn>
		requires std::is_invocable_r_v<T, Fn, size_t, size_t>
	void ForEach(Fn&& func)
	{
		for (size_t i = 0; i < m_rows; ++i)
		{
			for (size_t j = 0; j < m_cols; ++j)
			{
				m_data[i][j] = func(i, j);
			}
		}
	}

	const std::vector<T>& operator[](size_t row) const
	{
		return const_cast<Matrix&>(*this)[row];
	}

	size_t GetRowsCount() const { return m_rows; }
	size_t GetColumnsCount() const { return m_cols; }
	size_t GetSize() const { return std::max(m_rows, m_cols); }

private:
	size_t m_rows;
	size_t m_cols;
	std::vector<std::vector<T>> m_data;
};
