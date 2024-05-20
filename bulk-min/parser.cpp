#include "parser.h"

using namespace std;

parser::parser()
{}

database parser::load(string path)
{
	m_file_path = path;
	vector<string> strings;

	if (try_all())
	{
		log::success(PARSER, "Parsing process finished succsessfully");
	}
	else
	{
		log::error(PARSER, "Parsing process aborted with errors");
	}

	return m_data;
}

bool parser::try_all()
{
	header header{};
	vector<string> text;

	if (!try_read(text))
		return false;

	if (!try_parse(text))
		return false;

	if (!try_read_header(text, header))
		return false;

	m_data = database(header);

	if (!try_read_data(text, m_data))
		return false;

	return true;
}

bool parser::try_read(std::vector<std::string>& vec) const
{
	string line = "";

	ifstream input_file;
	input_file.open(m_file_path);

	if (!input_file.is_open())
	{
		log::error(PARSER, "File could not be open");

		return false;
	}

	while (getline(input_file, line))
	{
		vec.push_back(line);
	}

	log::success(PARSER, "File readed successfuly");
	input_file.close();

	return true;
}

bool parser::try_parse(std::vector<std::string>& vec) const
{
	int grid_count = 0;
	int tube_count = 0;
	int temp_count = 0;
	int slash_count = 0;

	for (int i = 0; i < vec.size(); i++)
	{
		if (vec[i].find("/") != string::npos)
		{
			vec[i].erase(vec[i].begin() + vec[i].find("/"), vec[i].end());
		}

		if (vec[i].find("--") != string::npos)
		{
			vec[i].erase(vec[i].begin() + vec[i].find("--"), vec[i].end());
		}

		if (vec[i][0] == ' ')
		{
			while (vec[i][0] == ' ')
			{
				vec[i].erase(vec[i].begin());
			}
		}

		if (vec[i] != "")
		{
			if (vec[i][vec[i].size() - 1] == ' ')
			{
				while (vec[i][vec[i].size() - 1] == ' ')
				{
					vec[i].pop_back();
				}
			}
		}

		if (vec[i] != "")
		{
			vec[i].push_back('/');
		}
	}

	for (int i = 0; i < vec.size(); i++)
	{
		if (vec[i] == "")
		{
			vec.erase(vec.begin() + i);
		}
	}

	for (int i = 0; i < vec.size(); i++)
	{
		for (int j{}; j <= vec[i].length() - 4; )
		{
			int position = vec[i].find("GRID/", j);
			if (position == string::npos) break;
			grid_count++;
			j = position + 1;
		}
		for (int j{}; j <= vec[i].length() - 4; )
		{
			int position = vec[i].find("TUBE/", j);
			if (position == string::npos) break;
			tube_count++;
			j = position + 1;
		}
		for (int j{}; j <= vec[i].length() - 4; )
		{
			int position = vec[i].find("TEMP/", j);
			if (position == string::npos) break;
			temp_count++;
			j = position + 1;
		}
		for (int j{}; j <= vec[i].length() - 1; )
		{
			int position = vec[i].find("/", j);
			if (position == string::npos) break;
			slash_count++;
			j = position + 1;
		}
	}

	if (grid_count == 0)
	{
		cout << "\nWarning! GRID header was not found\n" << endl;
	}
	if (tube_count == 0)
	{
		cout << "\nWarning! TUBE header was not found\n" << endl;
	}
	if (temp_count == 0)
	{
		cout << "\nWarning! TEMP header was not found\n" << endl;
	}

	if (grid_count > 1)
	{
		cout << "\nWarning! More then one GRID header was found\n" << endl;
	}
	if (tube_count > 1)
	{
		cout << "\nWarning! More then one TUBE header was found\n" << endl;
	}
	if (temp_count > 1)
	{
		cout << "\nWarning! More then one TEMP header was found\n" << endl;
	}
	if (slash_count > 6)
	{
		cout << "\nWarning! Too much / markers\n" << endl;
	}
	if (slash_count < 6)
	{
		cout << "\nWarning! Not enough / markers\n" << endl;
	}

	if (grid_count == 1 && tube_count == 1 && temp_count == 1 && slash_count == 6)
	{
		log::success(PARSER, "Headers test 1 passed");
	}
	else
	{
		log::error(PARSER, "Headers test 1 failed");

		return false;
	}

	vector<string> temp_vec;
	temp_vec = vec;
	vec.clear();
	vec.push_back("");
	vec.push_back("");
	vec.push_back("");

	for (size_t i = 0; i < temp_vec.size(); i++)
	{
		if (temp_vec[i] == "GRID/")
		{
			vec[0] = temp_vec[i + 1];
		}
		if (temp_vec[i] == "TUBE/")
		{
			vec[1] = temp_vec[i + 1];
		}
		if (temp_vec[i] == "TEMP/")
		{
			vec[2] = temp_vec[i + 1];
		}
	}

	return true;
}

bool parser::try_read_header(std::vector<std::string>& vec, header& header) const
{
	int i = 0;
	int j = 0;
	string temp_line = "";

	while (i < vec[0].size())
	{
		if (vec[0][i] != ' ' && vec[0][i] != '/')
		{
			temp_line += vec[0][i];
		}
		else
		{
			switch (j)
			{
			case 0:
				header.length = stod(temp_line);
				break;
			case 1:
				header.space_segments = stoi(temp_line);
				break;
			case 2:
				header.time_segments = stoi(temp_line);
				break;
			default:
				log::warning(PARSER, "More then 3 arguments in GRID section");

				return false;
			}
			j++;
			temp_line = "";
		}
		i++;
	}

	log::success(PARSER, "Headers infilled successfully");

	return true;
}

bool parser::try_read_data(std::vector<std::string>& vec, database& data)
{
	int i = 0;
	int j = 0;
	string temp_line = "";
	while (i < vec[1].size())
	{
		if (vec[1][i] == ' ' || vec[1][i] == '/')
		{
			j++;
		}
		i++;
	}
	if (j != data.m_header.space_segments)
	{
		cout << "\nWarning! Wrong nuber of conductivity coefficients\n" << endl;

		return false;
	}

	i = 0;
	j = 0;
	temp_line = "";
	while (i < vec[1].size())
	{
		if (vec[1][i] != ' ' && vec[1][i] != '/')
		{
			temp_line += vec[1][i];
		}
		else
		{
			data.m_conductivities[j] = stod(temp_line);
			temp_line = "";
			j++;
		}
		i++;
	}
	if (j == data.m_header.space_segments)
	{
		cout << "Coefficients infilled successfully" << endl;
	}

	i = 0;
	j = 0;
	int k = data.m_header.space_segments;
	temp_line = "";

	while (i < vec[2].size())
	{
		if (vec[2][i] == ' ' || vec[2][i] == '/')
		{
			if (k == data.m_header.space_segments)
			{
				k = 0;
			}
			else
			{
				j++;
				k++;
			}
		}
		i++;
	}
	if (k != data.m_header.space_segments || j != (data.m_header.space_segments) * (data.m_header.time_segments))
	{
		cout << "\nWarning! Wrong number of temperatures\n" << endl;

		return false;
	}

	i = 0;
	j = 0;
	k = data.m_header.space_segments;
	temp_line = "";

	while (i < vec[2].size())
	{
		if (vec[2][i] != ' ' && vec[2][i] != '/')
		{
			temp_line += vec[2][i];
		}
		else
		{
			if (k == data.m_header.space_segments)
			{
				data.m_time_scale[j / data.m_header.space_segments] = stod(temp_line);
				k = 0;
				temp_line = "";
			}
			else
			{
				data.m_temperatures[j] = stod(temp_line);
				temp_line = "";
				j++;
				k++;
			}
		}
		i++;
	}
	if (k == data.m_header.space_segments && j == (data.m_header.space_segments) * (data.m_header.time_segments))
	{
		cout << "Temperatures infilled successfully" << endl;
	}

	return true;
}
