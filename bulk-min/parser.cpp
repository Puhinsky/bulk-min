#include "parser.h"

using namespace std;

parser::parser()
{}

//starts try_all() function
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

//serially starts functions, working with file
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

	if(!verificator(m_data))
		return false;

	return true;
}

//opens file
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

	//line-by-line reading of the file
	while (getline(input_file, line))
	{
		vec.push_back(line);
	}

	log::success(PARSER, "File readed successfuly");
	input_file.close();

	return true;
}

//delete excess strings, check correction of strings
bool parser::try_parse(std::vector<std::string>& vec) const
{
	int grid_count = 0;
	int tube_count = 0;
	int temp_count = 0;
	int slash_count = 0;

	//modifying of each line in vector
	for (int i = 0; i < vec.size(); i++)
	{
		//delete all text after block end symbol "/"
		if (vec[i].find("/") != string::npos)
		{
			vec[i].erase(vec[i].begin() + vec[i].find("/"), vec[i].end());
		}

		//delete comment section
		if (vec[i].find("--") != string::npos)
		{
			vec[i].erase(vec[i].begin() + vec[i].find("--"), vec[i].end());
		}

		//delete spaces at beginning of the line
		if (vec[i][0] == ' ')
		{
			while (vec[i][0] == ' ')
			{
				vec[i].erase(vec[i].begin());
			}
		}

		//delete spaces at end of the line
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

		//add srting end symbol "/"
		if (vec[i] != "")
		{
			vec[i].push_back('/');
		}

		//erase excess spaces
		int j = 0;
		if (vec[i] != "")
		{
			while (vec[i][j] != '/')
			{
				if (vec[i][j] == ' ' && vec[i][j + 1] == ' ')
				{
					vec[i].erase(vec[i].begin() + j);
				}
				else
				{
					j++;
				}
			}
		}
		
	}

	//delete all empty lines
	for (int i = 0; i < vec.size(); i++)
	{
		if (vec[i] == "")
		{
			vec.erase(vec.begin() + i);
		}
	}

	//counts strings with data blocks beginning keywords
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

	if (grid_count != 1)
	{
		log::error(PARSER, "Wrong number of GRID keywords");
		return false;
	}
	if (temp_count != 1)
	{
		log::error(PARSER, "Wrong number of TEMP keywords");
		return false;
	}
	if (slash_count != 6 && slash_count != 4)
	{
		log::error(PARSER, "Wrong number of sections or empty section");
		return false;
	}
	if (tube_count > 1)
	{
		log::error(PARSER, "Wrong number of TUBE keywords");
		return false;
	}
	if (tube_count == 0)
	{
		log::info(PARSER, "File without TUBE section");
	}

	log::success(PARSER, "Keywords test passed");

	vector<string> temp_vec;
	temp_vec = vec; 
	vec.clear();
	vec.push_back("");
	vec.push_back("");
	vec.push_back("");
	//now "vec" is vector of 2 or 3 empty strings

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

//infills header sructure
bool parser::try_read_header(std::vector<std::string>& vec, header& header) const
{
	int i = 0;
	int j = 0;
	int k = 0;
	string temp_line = "";

	while (i < vec[0].size())
	{
		if (vec[0][i] != ' ' && vec[0][i] != '/')
		{
			temp_line += vec[0][i];
			k++;
		}
		else
		{
			if (k > 9)
			{
				log::error(PARSER, "Too big value of argument in GRID section");
				return false;
			}
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
				log::error(PARSER, "More then 3 arguments in GRID section");

				return false;
			}
			j++;
			k = 0;
			temp_line = "";
		}
		i++;
	}

	//checking the correctness of the data in header
	if (header.length <= 0)
	{
		log::error(PARSER, "Wrong bulk's lenght");
		return false;
	}
	if (header.space_segments > 10000 || header.space_segments < 2)
	{
		log::error(PARSER, "Wrong nuber of space segments");
		return false;
	}
	if (header.time_segments > 1000 || header.time_segments < 2)
	{
		log::error(PARSER, "Wrong nuber of time segments");
		return false;
	}


	log::success(PARSER, "Headers infilled successfully");

	return true;
}

//infills data structure
bool parser::try_read_data(std::vector<std::string>& vec, database& data)
{
	int i = 0;
	int j = 0;
	string temp_line = "";

	//counts number of elements in TUBE section if this section exists
	if (vec[1].size() > 0)
	{
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
			log::error(PARSER, "Wrong nuber of elements in TUBE section");
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
			log::success(PARSER, "TUBE section infilled successfully");
		}
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
		log::error(PARSER, "Wrong nuber of elements in TEMP section");
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
		log::success(PARSER, "TEMP section infilled successfully");
	}

	return true;
}

//extra data tests
bool parser::verificator(database& data)
{
	//checks order of time segments
	for (int i = 1; i < data.m_header.time_segments; i++)
	{
		if (data.m_time_scale[i] <= data.m_time_scale[i-1])
		{
			log::error(PARSER, "Wrong order of time segments");
			return false;
		}
	}

	//all initial temperatures should be declared
	for (int i = 0; i < data.m_header.space_segments; i++)
	{
		if (data.m_temperatures[i] == -1)
		{
			log::error(PARSER, "Initial temperature(s) missed");
			return false;
		}
	}

	//temperatures at edges of bulck should be always declared
	for (int i = data.m_header.space_segments - 1; i < (data.m_header.space_segments) * (data.m_header.time_segments) - data.m_header.space_segments; i += data.m_header.space_segments)
	{
		if (data.m_temperatures[i] == -1 || data.m_temperatures[i + 1] == -1)
		{
			log::error(PARSER, "Edge temperature(s) missed");
			return false;
		}
	}
	if (data.m_temperatures[0] == -1 || data.m_temperatures[(data.m_header.space_segments) * (data.m_header.time_segments) - 1] == -1)
	{
		log::error(PARSER, "Edge temperature(s) missed");
		return false;
	}

	//checks conductivity coefficients
	for (int i = 0; i < data.m_header.space_segments; i++)
	{
		//cout << data.m_conductivities[i] << endl;
		if (data.m_conductivities[i] < 0)
		{
			log::error(PARSER, "Wrong value(s) of conductivity coeffecient(s)");
			return false;
		}
	}

	log::success(PARSER, "Data verification test passed successfully");

	return true;
}
