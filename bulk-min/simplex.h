#pragma once

#include "base_method.h"

class simplex : public base_method
{
private:
	size_t m_points_count;
	size_t m_dimension;
	double m_l;
	double m_sigma;
	double m_epsilon;
	double** m_x;
	double* m_y;

	double* m_reflected;
	double* m_center;
	std::map<size_t, double> m_reflected_map;

	void build_simplex(const double* base_vertex);
	void print_x() const;
	void print_y() const;
	size_t find_index_of_max_point() const;
	bool try_reflect();
	void reflect(size_t index);
	void compute_center_for_reflect(size_t index);
	void reduce();
	size_t find_index_of_min_reflected_point() const;

protected:
	void on_init() override;
	void on_iteration() override;
	void on_termination() override;
	bool termination_condition() override;

public:
	simplex();

	void set_step(double value);
	void set_step_divider(double value);
	void set_min_step(double value);
};

