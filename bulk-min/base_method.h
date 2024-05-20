#pragma once

#include "database.h"
#include <xmemory>

class base_method
{
private:
	bool m_round_enable;
	std::shared_ptr<database> m_data;
	std::unique_ptr<double[]> m_result_temperatures;
	std::unique_ptr<double[]> m_error;
	std::unique_ptr<double[]> m_p;
	std::unique_ptr<double[]> m_q;
	double m_delta_space_multiplier;
	void copy_first_layer();
	void calculate_layer(size_t layer);
	inline double calculate_sigma(size_t layer);
	inline void calculate_pq_of_first_point(size_t layer);
	void calculate_pq_of_other_point(double sigma);
	inline void back_propagation(size_t layer);
	inline size_t get_temperatures_index(size_t layer, size_t space_number);
	void evaluate_error(size_t layer, size_t index);

protected:
	std::unique_ptr<double[]> compute_task();
	virtual void on_init();
	virtual void on_iteration();
	virtual void on_termination();
	virtual bool termination_condition();

public:
	base_method();
	void run(std::shared_ptr<database> data);
	void set_round_enable(bool value);
	bool get_round_enable() const;
};

