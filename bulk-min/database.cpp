#include "database.h"

using namespace std;

std::string database::serialize_conductivities() const
{
	string conductivities;

	for (size_t i = 0; i < m_header.space_segments; i++)
	{
		conductivities += to_string(m_conductivities[i]);

		if (i != m_header.space_segments - 1)
			conductivities += " ";
	}

	return conductivities;
}

std::string database::serialize_temperatures() const
{
	string temperatures;

	for (size_t i = 0; i < m_header.time_segments; i++)
	{
		for (size_t j = 0; j < m_header.space_segments; j++)
		{
			temperatures += to_string(m_temperatures[i * m_header.space_segments + j]);

			if (j != m_header.space_segments - 1)
				temperatures += " ";
		}

		if (i != m_header.time_segments - 1)
			temperatures += "\n";
	}

	return temperatures;
}

std::string database::serialize_time_scale() const
{
	string time_scale;

	for (size_t i = 0; i < m_header.time_segments; i++)
	{
		time_scale += to_string(m_time_scale[i]);

		if (i != m_header.space_segments - 1)
			time_scale += " ";
	}

	return time_scale;
}

database::database()
{
	m_header = {0};
	m_conductivities = new double;
	m_temperatures = new double;
	m_time_scale = new double;
}

database::database(header header)
{
	m_header = header;
	m_conductivities = new double[header.space_segments];
	m_temperatures = new double[header.space_segments * header.time_segments];
	m_time_scale = new double[header.time_segments];

	//Fill with zeros
	for (int i = 0; i < header.space_segments; i++)
		m_conductivities[i] = 0.;
}

database::~database()
{
	/*delete[] m_conductivities;
	delete[] m_temperatures;
	delete[] m_time_scale;*/
}

void database::randomize_conductivities(double from, double to) const
{
	uniform_real_distribution<double> unif(from, to);
	default_random_engine re;

	for (size_t i = 0; i < m_header.space_segments; i++)
	{
		m_conductivities[i] = unif(re);
	}
}

void database::print() const
{
	log::info(DATABASE, "Bulk lenght: " + to_string(m_header.length)
		+ " m.\n\tSpace segments: " + to_string(m_header.space_segments)
		+ "\n\tTime segments: " + to_string(m_header.time_segments));

	log::info(DATABASE, " Conductivities:\n" + serialize_conductivities());
	log::info(DATABASE, " Temperatures:\n" + serialize_temperatures());
	log::info(DATABASE, " Time scale:\n" + serialize_time_scale());
}
