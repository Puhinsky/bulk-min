#pragma once

#include "base_method.h"

using namespace std;

class genetic : public base_method
{
private:
	size_t m_iteration_count;
	size_t m_population_size;
	double m_step;
	double m_step_divider;

	//Too many internal stuff
	struct generation {
		double error;
		double* conductivity;
	};

	size_t m_iter;
	double m_L;
	double m_erC;
	double m_oldError;
	vector<generation> m_populations;

	double m_A;
	double m_B;
	double m_C;
	double m_S;
	double m_M;

	uniform_real_distribution<double> m_unif01;
	uniform_real_distribution<double> m_unif11;
	default_random_engine m_re;
	clock_t m_internalClock;

	void build_population();
	bool sorting_function(generation const& lhs, generation const&  rhs);

protected:
	void on_init() override;
	void on_iteration() override;
	void on_termination() override;
	bool termination_condition() override;

public:
	genetic();

	void set_iteration_count(size_t value);
	void set_population_size(size_t value);
	void set_step(double value);
	void set_step_divider(double value);
};
