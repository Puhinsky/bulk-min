#pragma once

#include "base_method.h"

using namespace std;

class cycle : public base_method
{
private:
	size_t m_erosion;
	double m_step;
	double m_step_divider;
	double m_epsilon;

	double m_L;

	void cycle_search(size_t start_index, size_t end_index);
	size_t find_largest_index(double* values);
protected:
	void on_init() override;
	void on_iteration() override;
	void on_termination() override;
	bool termination_condition() override;

public:
	cycle();

	void set_erosion(size_t value);
	void set_step(double value);
	void set_step_divider(double value);
	void set_epsilon(double value);
};
