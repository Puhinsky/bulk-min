#include "cycle.h"

using namespace std;

cycle::cycle()
{
	m_erosion = 2;
	m_step = 0.1;
	m_step_divider = 2.;
	m_epsilon = 0.01;
}

void cycle::set_erosion(size_t value)
{
	m_erosion = value;
}

void cycle::set_step(double value)
{
	m_step = value;
}

void cycle::set_step_divider(double value)
{
	m_step_divider = value;
}

void cycle::set_epsilon(double value)
{
	m_epsilon = value;
}

void cycle::cycle_search(size_t start_index, size_t end_index)
{
	auto conductivity = m_data.get()->m_conductivities;

	auto error = compute_task_sum(conductivity);
	int index = start_index;
	while (index <= end_index)
	{
		printf("\033[36m[INFO] Error: %f | L: %f\n\033[0m", error, m_L);

		auto initial_c = conductivity[index];
		conductivity[index] += m_L;
		auto new_error = compute_task_sum(conductivity);

		if (new_error < error) {
			error = new_error;
			continue;
		}

		conductivity[index] = initial_c;

		conductivity[index] -= m_L;
		conductivity[index] = max(conductivity[index], .0);


		new_error = compute_task_sum(conductivity);

		if (new_error < error) {
			error = new_error;
			continue;
		}
		
		conductivity[index] = initial_c;

		index++;
	}

	m_L /= m_step_divider;
}

size_t cycle::find_largest_index(double* values)
{
	double biggestValue = 0;
	size_t index = 0;

	for (int i = 0; i < m_data.get()->m_header.space_segments; i++)
	{
		if (values[i] > biggestValue)
		{
			biggestValue = values[i];
			index = i;
		}
	}

	return index;
}

void cycle::on_init()
{
	m_L = m_step;
}

void cycle::on_iteration()
{
	auto conductivity = m_data.get()->m_conductivities;
	auto errors = compute_task(conductivity);

	size_t index = find_largest_index(errors);
	size_t segments = m_data.get()->m_header.space_segments;

	size_t start_index = max(min(index - m_erosion, segments - 1), 0);
	size_t end_index = max(min(index + m_erosion, segments - 1), 0);

	cycle_search(start_index, end_index);
}

void cycle::on_termination()
{}

bool cycle::termination_condition()
{
	return (m_L <= m_epsilon);
}