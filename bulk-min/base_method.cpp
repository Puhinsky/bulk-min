#include "base_method.h"

using namespace std;

void base_method::copy_first_layer()
{
	for (size_t i = 0; m_data.get()->m_header.space_segments; i++)
	{
		m_result_temperatures[i] = m_data.get()->m_temperatures[i];
	}
}

void base_method::calculate_layer(size_t layer)
{
	double sigma = calculate_sigma(layer);
	calculate_pq_of_first_point(layer);
	calculate_pq_of_other_point(layer);
	back_propagation(layer);
}

inline double base_method::calculate_sigma(size_t layer)
{
	double delta_time = m_data.get()->m_time_scale[layer] - m_data.get()->m_time_scale[layer - 1];

	return delta_time / m_delta_space_multiplier;
}

inline void base_method::calculate_pq_of_first_point(size_t layer)
{
	m_p[0] = 0;
	m_q[0] = m_data.get()->m_temperatures[get_temperatures_index(layer - 1, 0)];
}

void base_method::calculate_pq_of_other_point(double sigma)
{
	for (size_t i = 1; i < m_data.get()->m_header.space_segments; i++)
	{
		auto cond_left = m_data.get()->m_conductivities[i - 1];
		auto cond_center = m_data.get()->m_conductivities[i];
		auto cond_right = m_data.get()->m_conductivities[i + 1];

		auto a = -sigma * (cond_center + cond_left);
		auto b = 1 + sigma * (cond_left + 2 * cond_center + cond_right);
		auto c = -sigma * (cond_center + cond_right);
		auto d = m_result_temperatures[i];

		m_p[i] = -c / (a * m_p[i - 1] + b);
		m_q[i] = (d - a * m_q[i - 1]) / (a * m_p[i - 1] + b);
	}
}

inline void base_method::back_propagation(size_t layer)
{
	for (size_t i = m_data.get()->m_header.space_segments - 2; i > 0; i--)
	{
		m_result_temperatures[i] = m_p[i] * m_result_temperatures[i + 1] + m_q[i];
		evaluate_error(layer, i);
	}
}

inline size_t base_method::get_temperatures_index(size_t layer, size_t space_number)
{
	return layer * m_data.get()->m_header.space_segments + space_number;
}

void base_method::evaluate_error(size_t layer, size_t index)
{
	auto temperature_index = get_temperatures_index(layer, index);

	if (m_data.get()->m_conductivities[temperature_index] < 0.0)
		return;

	m_error[index] += pow(m_result_temperatures[index] - m_data.get()->m_conductivities[temperature_index], 2);
}

std::unique_ptr<double[]> base_method::compute_task()
{
	copy_first_layer();

	for (size_t i = 1; i < m_data.get()->m_header.time_segments; i++)
	{
		calculate_layer(i);
	}

	return move(m_error);
}

base_method::base_method()
{
	m_round_enable = false;
	m_delta_space_multiplier = 0.0;
}

void base_method::run(shared_ptr<database> data)
{
	m_data = move(data);
	m_result_temperatures = make_unique<double[]>(m_data.get()->m_header.space_segments);
	m_delta_space_multiplier = 2 * pow(m_data.get()->m_header.length / m_data.get()->m_header.space_segments, 2);
	m_error = make_unique<double[]>(m_data.get()->m_header.space_segments);
	m_p = make_unique<double[]>(m_data.get()->m_header.space_segments - 1);
	m_q = make_unique<double[]>(m_data.get()->m_header.space_segments - 1);

	on_init();

	do
	{
		on_iteration();
	} while (termination_condition());

	on_termination();
}

void base_method::set_round_enable(bool value)
{
	m_round_enable = value;
}

bool base_method::get_round_enable() const
{
	return m_round_enable;
}

void base_method::on_init()
{}

void base_method::on_iteration()
{}

void base_method::on_termination()
{}

bool base_method::termination_condition()
{
	return false;
}
