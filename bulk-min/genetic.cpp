#include "genetic.h"

using namespace std;

genetic::genetic()
{
	m_population_size = 20;
	m_iteration_count = 10;
	m_step = .1;
	m_step_divider = 2.;
}

void genetic::set_iteration_count(size_t value)
{
	m_iteration_count = value;
}

void genetic::set_population_size(size_t value)
{
	m_population_size = value;
}

void genetic::set_step(double value)
{
	m_step = value;
}

void genetic::set_step_divider(double value)
{
	m_step_divider = value;
}

void genetic::build_population()
{
	size_t array_elements = m_data.get()->m_header.space_segments;
	size_t array_size = array_elements * sizeof(double);
	double* conductivity = m_populations[0].conductivity;
	memcpy_s(conductivity, array_size, m_data.get()->m_conductivities, array_size);

	uniform_real_distribution<double> unif(-m_L, m_L);
	default_random_engine re(time(0));

	for (int p = 1; p < m_population_size; p++)
		for (int i = 0; i < array_elements; i++)
			m_populations[p].conductivity[i] = abs(conductivity[i] + unif(re));
}

void genetic::on_init()
{
	m_populations = vector<generation>(m_population_size);
	for (int p = 0; p < m_population_size; p++)
	{
		m_populations[p].conductivity = new double[m_data.get()->m_header.space_segments];
	}

	build_population();

	m_iter = m_iteration_count;
	m_erC = .0001;
	m_oldError = .0;

	m_A = 0.25;
	m_B = 0.40;
	m_C = 0.5;
	m_S = 0.5;
	m_M = 0.40;
	m_L = m_step;

	//Prepare random generator
	m_unif01 = uniform_real_distribution<double>(0, 1);
	m_unif11 = uniform_real_distribution<double>(-1, 1);
	m_re = default_random_engine(time(0));

	m_internalClock = clock();
}

void genetic::on_iteration()
{
	for (int p = 0; p < m_population_size; p++)
	{
		m_populations[p].error = compute_task_sum(m_populations[p].conductivity);
	}

	/*
bool genetic::sorting_function(generation const& lhs, generation const& rhs)
{
	return lhs.error < rhs.error;
}
	*/

	sort(m_populations.begin(), m_populations.end(), [](auto lhs, auto rhs) {return lhs.error < rhs.error;});

	if (m_erC >= 1.1)
	{
		m_erC = .01;
		m_L /= 2.;
		build_population();
		m_L /= 5.;
	}
	else if (m_oldError != m_populations[0].error)
	{
		double delta = abs(m_populations[0].error - m_oldError);
		m_oldError = m_populations[0].error;
		//erC /= 2. * max(1,erC*2);
		m_erC /= max(0.95, ((delta / m_L / m_step_divider) / m_oldError) * 14);
		m_erC = min(m_erC, 1.2);
	}
	else
	{
		m_erC += min(0.05, 0.009 / m_erC / max(m_L, 0.0000000001));
	}

	if (m_L == 0.)
	{
		m_iter--;
		return;
	}

	if (m_erC < 0.003)
		m_L *= 1.01;

	int lA = round(max(m_population_size * max(m_A - m_erC / 4., .04), 4));
	int lB = round(m_population_size * max(m_B - m_erC / 5, .08));

	for (int j = lA; j < lB; j++)
	{
		int tmp1 = round((m_unif01(m_re) * (lA - 1)));
		auto tube1 = m_populations[tmp1];
		int tmp2 = 0;
		do
		{
			tmp2 = round((m_unif01(m_re) * (lA - 1)));
		} while (tmp2 == tmp1);
		auto tube2 = m_populations[tmp2];

		for (int k = 0; k < m_data.get()->m_header.space_segments; k++)
			m_populations[j].conductivity[k] = m_unif01(m_re) < m_C ? tube1.conductivity[k] : tube2.conductivity[k];
	}

	for (int j = lB; j < m_population_size; j++) 
	{
		int tmp = min(round(m_unif01(m_re) * min((m_S + m_erC), m_B) * m_population_size), lB - 1);
		auto tube = m_populations[tmp];

		for (int k = 0; k < m_data.get()->m_header.space_segments; k++)
			m_populations[j].conductivity[k] = abs(tube.conductivity[k] + 
				(m_unif01(m_re) < min(m_M + m_erC, 0.95) ? m_unif11(m_re) * 
				(m_L * (1 + m_erC) / m_step_divider) : 0.));
	}

	printf("Remaining iterations: %d | erC: %.3f | L: %.6f | Error: %f | Remaining time: %.1f s\n", m_iteration_count,
		m_erC, m_L, m_populations[0].error, ((double)(clock() - m_internalClock)) * m_iter / 10 / CLOCKS_PER_SEC);

	m_internalClock = clock();

	m_iter--;
}

bool genetic::termination_condition()
{
	return (m_iter == 0);
}

void genetic::on_termination()
{
	for (int p = 0; p < m_population_size; p++)
		delete m_populations[p].conductivity;
}
