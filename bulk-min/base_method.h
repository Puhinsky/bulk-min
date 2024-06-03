#pragma once

#include "database.h"
#include "log.h"
#include <xmemory>

#define min(a,b) (a < b ? a : b)
#define max(a,b) (a > b ? a : b)

class base_method
{
private:
	bool m_round_enable;
	double* m_conductivities;
	double* m_result_temperatures;
	double* m_error;
	double* m_p;
	double* m_q;
	double m_delta_space_multiplier;
	void copy_first_layer();
	void calculate_layer(size_t layer);
	inline double calculate_sigma(size_t layer);
	inline void calculate_pq_of_first_point(size_t layer);
	void calculate_pq_of_other_point(double sigma);
	inline void back_propagation(size_t layer);
	inline size_t get_temperature_index(size_t layer, size_t space_number);
	void evaluate_error(size_t layer, size_t index);
	void print_temperatures() const;
	void print_error() const;
	void init_errors();

protected:
	std::shared_ptr<database> m_data;
	double* compute_task(double* conductivities);
	double compute_task_sum(double* conductivities);
	void compute_task_errors(double* conductivities, double** errors);
	virtual void on_init();
	virtual void on_iteration();
	virtual void on_termination();
	virtual bool termination_condition();

public:
	base_method();
	~base_method();
	void run(std::shared_ptr<database> data);
	void set_round_enable(bool value);
	bool get_round_enable() const;
	double test_run(std::shared_ptr<database> data);
};

