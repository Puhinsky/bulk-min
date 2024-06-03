#pragma once

#include "database.h"
#include "log.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cctype>

class parser
{
private:
	std::string m_file_path;
	database m_data;

	bool try_all();
	bool try_read(std::vector<std::string>& vec) const;
	bool try_parse(std::vector<std::string>& vec) const;
	bool try_read_header(std::vector<std::string>& vec, header& header) const;
	bool try_read_data(std::vector<std::string>& vec, database& data);
	bool verificator(database& data);

public:
	parser();
	database load(std::string path);

};
