#pragma once

#include "base_method.h"

using namespace std;

class viewdata : public base_method
{
private:

	const double max_grid_space = 10;
	const double max_grid_time = 20;

	double** m_errors;
	double grid_space;
	double grid_time;
	int grid_space_start;
	int grid_time_start;

	int grid_space_end;
	int grid_time_end;

	double legend_max;
	double legend_min;

	void update_legend();
	double get_max(size_t space_index, size_t time_index);
	void heat_print(int level);
	void print_grid();
protected:
	void on_init() override;
	void on_iteration() override;
	void on_termination() override;
	bool termination_condition() override;

public:
	viewdata();
};
