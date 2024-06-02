#pragma once

#include <string>
#include <map>
#include <iostream>

enum log_source_e
{
	PARSER,
	EXPORTER,
	GENETIC,
	CYCLE,
	SIMPLEX,
	BASE_METHOD,
	DATABASE,
	CLI
};

class log
{
private:
	static std::map<log_source_e, std::string> m_source_map;

	static void print(const log_source_e source, const std::string& text);
	static void set_text_color(const std::string& color_sequence);
	static void reset_text_color();

public:
	static void info(const log_source_e source, const std::string& text);
	static void success(const log_source_e source, const std::string& text);
	static void warning(const log_source_e source, const std::string& text);
	static void error(const log_source_e source, const std::string& text);
};
