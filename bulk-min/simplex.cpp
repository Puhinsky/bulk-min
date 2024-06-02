#include "simplex.h"

using namespace std;

simplex::simplex()
{
	m_points_count = 0;
	m_l = 0.1;
	m_sigma = 0.95;
	m_epsilon = 0.00001;
	m_x = new double*;
	m_y = new double;
}

void simplex::set_step(double value)
{
	m_l = value;
}

void simplex::set_step_divider(double value)
{
	m_sigma = value;
}

void simplex::set_min_step(double value)
{
	m_epsilon = value;
}

void simplex::build_simplex(const double* base_vertex)
{
	m_x = new double* [m_points_count];

	for (size_t i = 0; i < m_points_count; i++)
	{
		m_x[i] = new double[m_dimension];
	}

	m_y = new double[m_points_count];

	for (size_t i = 0; i < m_dimension; i++)
	{
		m_x[0][i] = base_vertex[i];
	}

	double sqrt_n1 = sqrt(m_dimension + 1);

	for (size_t i = 1; i < m_points_count; i++)
	{
		for (size_t j = 0; j < m_dimension; j++)
		{
			if (i == j + 1)
				m_x[i][j] = base_vertex[j] + (sqrt_n1 - 1) / (m_dimension * sqrt(2)) * m_l;
			else
				m_x[i][j] = base_vertex[j] + (sqrt_n1 + m_dimension - 1) / (m_dimension * sqrt(2)) * m_l;
		}
	}

	for (size_t i = 0; i < m_points_count; i++)
	{
		m_y[i] = compute_task_sum(m_x[i]);
	}
}

void simplex::print_x() const
{
	for (size_t i = 0; i < m_points_count; i++)
	{
		string x;

		for (size_t j = 0; j < m_dimension; j++)
		{
			x += to_string(m_x[i][j]);

			if (j != m_points_count - 1)
				x += " ";
		}

		log::info(SIMPLEX, x);
	}
}

void simplex::print_y() const
{
	string y;

	for (size_t i = 0; i < m_points_count; i++)
	{
		y += to_string(m_y[i]);

		if (i != m_points_count - 1)
			y += " ";
	}

	log::info(SIMPLEX, y);
}

size_t simplex::find_index_of_max_point() const
{
	size_t max_index = 0;
	double max_value = m_y[0];

	for (size_t i = 0; i < m_points_count; i++)
	{
		if (m_y[i] > max_value)
		{
			max_value = m_y[i];
			max_index = i;
		}
	}

	return max_index;
}

size_t simplex::find_index_of_min_point() const
{
	size_t min_index = 0;
	double min_value = m_y[0];

	for (size_t i = 0; i < m_points_count; i++)
	{
		if (m_y[i] < min_value)
		{
			min_value = m_y[i];
			min_index = i;
		}
	}

	return min_index;
}

bool simplex::try_reflect()
{
	auto max_index = find_index_of_max_point();
	reflect(max_index);
	double reflected_value = compute_task_sum(m_reflected);

	if (m_y[max_index] > reflected_value)
	{
		for (size_t i = 0; i < m_dimension; i++)
		{
			m_x[max_index][i] = m_reflected[i];
		}

		m_y[max_index] = reflected_value;

		return true;
	}

	m_reflected_map.clear();

	for (size_t i = 0; i < m_points_count; i++)
	{
		if (i == max_index)
			continue;

		reflect(i);
		reflected_value = compute_task_sum(m_reflected);

		if (m_y[i] > reflected_value)
		{
			m_reflected_map.insert({ i, reflected_value });
		}
	}

	if (!m_reflected_map.empty())
	{
		auto index_of_min = find_index_of_min_reflected_point();
		reflect(index_of_min);
		m_y[index_of_min] = compute_task_sum(m_reflected);

		for (size_t i = 0; i < m_dimension; i++)
		{
			m_x[index_of_min][i] = m_reflected[i];
		}

		return true;
	}

	return false;
}

void simplex::reflect(size_t index)
{
	compute_center_for_reflect(index);

	for (size_t i = 0; i < m_dimension; i++)
	{
		m_reflected[i] = m_center[i] - m_x[index][i];
	}
}

void simplex::compute_center_for_reflect(size_t index)
{
	for (size_t i = 0; i < m_dimension; i++)
	{
		m_center[i] = 0.0;
	}

	for (size_t i = 0; i < m_points_count; i++)
	{
		if (i != index)
		{
			for (size_t j = 0; j < m_dimension; j++)
			{
				m_center[j] += m_x[i][j];
			}
		}
	}

	for (size_t i = 0; i < m_dimension; i++)
	{
		m_center[i] *= 2.0 / m_dimension;
	}
}

void simplex::reduce()
{
	m_l *= m_sigma;

	for (size_t i = 1; i < m_points_count; i++)
	{
		for (size_t j = 0; j < m_dimension; j++)
		{
			m_x[i][j] = m_x[0][j] + m_sigma * (m_x[i][j] - m_x[0][j]);
		}
	}

	for (size_t i = 1; i < m_points_count; i++)
	{
		m_y[i] = compute_task_sum(m_x[i]);
	}
}

size_t simplex::find_index_of_min_reflected_point() const
{
	auto min_element = m_reflected_map.begin();

	for (auto it = m_reflected_map.begin(); it != m_reflected_map.end(); ++it)
	{
		if (it->second < min_element->second)
		{
			min_element = it;
		}
	}

	return min_element->first;
}

void simplex::on_init()
{
	m_dimension = m_data.get()->m_header.space_segments;
	m_points_count = m_dimension + 1;
	build_simplex(m_data.get()->m_conductivities);
	print_x();
	print_y();
	m_reflected = new double[m_dimension];
	m_center = new double[m_dimension];

	auto min_index = find_index_of_min_point();
	double deviation = compute_task_sum(m_x[min_index]);
	log::info(SIMPLEX, "begin deviation: " + to_string(deviation));
}

void simplex::on_iteration()
{
	print_y();

	if (try_reflect())
	{
		log::info(SIMPLEX, "Reflect");

		return;
	}

	reduce();
	log::info(SIMPLEX, "Reduce");
}

void simplex::on_termination()
{
	auto min_index = find_index_of_min_point();
	double deviation = compute_task_sum(m_x[min_index]);
	log::info(SIMPLEX, "final deviation: " + to_string(deviation));

	for (size_t i = 0; i < m_dimension; i++)
	{
		m_data.get()->m_conductivities[i] = m_x[min_index][i];
	}
}

bool simplex::termination_condition()
{
	return m_l < m_epsilon;
}
