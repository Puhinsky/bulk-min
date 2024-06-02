#pragma once

#include "header.h"
#include "log.h"
#include <memory>
#include <random>

class database
{
private:
	std::string serialize_conductivities() const;
	std::string serialize_temperatures() const;
	std::string serialize_time_scale() const;

public:
	header m_header;
	double* m_conductivities;
	double* m_temperatures;
	double* m_time_scale;

	database();
	database(header header);
	~database();
	void randomize_conductivities(double from = 0.0, double to = 1.0) const;
	void print() const;
};

