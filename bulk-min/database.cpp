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
