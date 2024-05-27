#pragma once

#include "header.h"
#include <memory>
#include <random>

class database
{
public:
	header m_header;
	std::shared_ptr<double[]> m_conductivities;
	std::shared_ptr<double[]> m_temperatures;
	std::shared_ptr<double[]> m_time_scale;

	database();
	database(header header);
	void randomize_conductivities(double from = 0.0, double to = 1.0) const;
};

