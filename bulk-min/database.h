#pragma once

#include <memory>
#include "header.h"

class database
{
public:
	header m_header;
	std::shared_ptr<double[]> m_conductivities;
	std::shared_ptr<double[]> m_temperatures;
	std::shared_ptr<double[]> m_time_scale;

	database();
	database(header header);
};

