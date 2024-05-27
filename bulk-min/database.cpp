#include "database.h"


using namespace std;

database::database()
{
	m_header = {0};
}

database::database(header header)
{
	m_header = header;
	m_conductivities = make_unique<double[]>(header.space_segments);
	m_temperatures = make_unique<double[]>(header.space_segments * header.time_segments);
	m_time_scale = make_unique<double[]>(header.time_segments);
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
