#include "viewdata.h"

viewdata::viewdata()
{

}

void viewdata::update_legend()
{
	legend_max = m_errors[grid_time_start][grid_space_start];
	legend_min = m_errors[grid_time_start][grid_space_start];

	for (size_t i = grid_time_start; i < grid_time_end; i++)
		for (size_t j = grid_space_start; j < grid_space_end; j++)
		{
			auto value = m_errors[i][j];

			if (value > legend_max)
				legend_max = value;

			if (value < legend_min)
				legend_min = value;
		}
}

void viewdata::heat_print(int level)
{
	level = min(max(level, 0), 5);
	const string colors[] = {
		"\033[35m",
		"\033[34m",
		"\033[36m",
		"\033[33m",
		"\033[32m",
		"\033[31m"
	};
	cout << colors[level] << "# " << "\033[0m";
}

double viewdata::get_max(size_t space_index, size_t time_index)
{
	int s_start = floor(grid_space_start + (space_index) * grid_space);
	int s_end = min(floor(grid_space_start + (space_index + 1) * grid_space),
		m_data.get()->m_header.space_segments);

	int t_start = floor(grid_time_start + (time_index) * grid_time);
	int t_end = min(floor(grid_time_start + (time_index + 1) * grid_time),
		m_data.get()->m_header.time_segments);

	double max_value = 0.;

	for (int i = t_start; i < t_end; i++)
		for (int j = s_start; j < s_end; j++)
			if (m_errors[i][j] > max_value)
				max_value = m_errors[i][j];

	return max_value;
}

void viewdata::print_grid()
{
	printf(" Space step: %6.1f | Time step: %6.1f\n\n", grid_space, grid_time);

	int m_grid_space = min((grid_space_end - grid_space_start) / grid_space, max_grid_space);
	int m_grid_time = min((grid_time_end - grid_time_start) / grid_time, max_grid_time);

	for (int j = -1; j < m_grid_space; j++)
	{
		if (j == -1)
		{
			cout << "   ";
			for (int i = 0; i < m_grid_time; i++)
			{
				int t_start = floor(grid_time_start + (i) * grid_time);
				int t_end = min(floor(grid_time_start + (i + 1) * grid_time),
					m_data.get()->m_header.time_segments);

				if (t_end - t_start > 1)
					cout << "\033[100m";
				else
					cout << "\033[0m";

				cout << (char)('A' + i) << ' ';
			}

			cout << "\033[0m";
		}
		else
		{
			int s_start = floor(grid_space_start + (j) * grid_space);
			int s_end = min(floor(grid_space_start + (j + 1) * grid_space),
				m_data.get()->m_header.space_segments);

			if (s_end - s_start > 1)
				cout << " \033[100m";
			else
				cout << " \033[0m";

			cout << j;

			cout << "\033[0m ";
			for (int i = 0; i < m_grid_time; i++)
			{
				auto value = get_max(j, i);
				auto level = (int)(6. * (double)(value - legend_min) / (double)(legend_max - legend_min));
				heat_print(level);
			}
		}
		cout << endl;
	}

	printf("\n Legend: %f \033[35m#\033[34m#\033[36m#\033[33m#\033[32m#\033[31m#\033[0m %f\n", legend_min, legend_max);
}

void viewdata::on_init()
{
	set_round_enable(true);

	grid_space_start = 0;
	grid_time_start = 0;

	grid_space_end = m_data.get()->m_header.space_segments;
	grid_time_end = m_data.get()->m_header.time_segments;

	grid_space = (grid_space_end - grid_space_start) / max_grid_space;
	grid_time = (grid_time_end - grid_time_start) / max_grid_time;

	if (grid_space <= 0.99)
		grid_space = 1;

	if (grid_time <= 0.99)
		grid_time = 1;

	m_errors = new double*[m_data.get()->m_header.time_segments];
	for (size_t i = 0; i < m_data.get()->m_header.time_segments; i++)
		m_errors[i] = new double[m_data.get()->m_header.space_segments];

	for (size_t i = 0; i < m_data.get()->m_header.space_segments; i++)
		m_errors[0][i] = 0.;

	compute_task_errors(m_data.get()->m_conductivities, m_errors);

	update_legend();
	print_grid();
}

void viewdata::on_iteration()
{
	string input;

	while (true)
	{
		cout << "VIZ>";
		getline(cin, input);

		if (input == "exit")
			break;

		char cmd = input[0];

		if (cmd == 'q')
			break;
		else if (cmd == 'r')
			on_init();
		else if (cmd >= '0' && cmd <= '9')
		{
			auto index = cmd - '0';
			auto count = (int)((grid_space_end - grid_space_start)/ grid_space);
			if (index >= count) {
				log::error(VIZ, "Space out of range.");
				continue;
			}

			int s_start = floor(grid_space_start + (index) * grid_space);
			int s_end = min(floor(grid_space_start + (index + 1) * grid_space),
				m_data.get()->m_header.space_segments);

			if (s_end - s_start <= 1) {
				log::error(VIZ, "Can't expand this region.");
				continue;
			}

			grid_space_start = s_start;
			grid_space_end = s_end;
			grid_space = (grid_space_end - grid_space_start) / max_grid_space;

			if (grid_space <= 0.99)
				grid_space = 1;

			update_legend();
			print_grid();
		}
		else if (cmd >= 'A' && cmd <= 'A'+19)
		{
			auto index = cmd - 'A';
			auto count = (int)((grid_time_end - grid_time_start)/ grid_time);
			if (index >= count) {
				log::error(VIZ, "Time out of range.");
				continue;
			}

			int t_start = floor(grid_time_start + (index) * grid_time);
			int t_end = min(floor(grid_time_start + (index + 1) * grid_time),
				m_data.get()->m_header.time_segments);

			if (t_end - t_start <= 1) {
				log::error(VIZ, "Can't expand this region.");
				continue;
			}

			grid_time_start = t_start;
			grid_time_end = t_end;
			grid_time = (grid_time_end - grid_time_start) / max_grid_time;

			if (grid_time <= 0.99)
				grid_time = 1;

			update_legend();
			print_grid();
		}
		else
		{
			log::error(VIZ, "Unknown command. Enter exit to return to CLI mode.");
		}
	}
}

void viewdata::on_termination()
{
	for (size_t i = 0; i < m_data.get()->m_header.time_segments; i++)
		delete m_errors[i];
	delete m_errors;
}

bool viewdata::termination_condition()
{
	return true;
}