#include "log.h"

using namespace std;

const string RESET = "\033[0m";
const string INFO_COLOR = "\033[36m";
const string SUCCESS_COLOR = "\033[32m";
const string WARNING_COLOR = "\033[33m";
const string ERROR_COLOR = "\033[31m";

map<log_source_e, string> log::m_source_map =
{
	{PARSER, "Parser" },
	{EXPORTER, "Exporter"},
	{GENETIC, "Genetic"},
	{CYCLE, "Cycle"},
	{SIMPLEX, "Simplex"},
	{BASE_METHOD, "Base method"},
	{DATABASE, "Database"},
	{CLI, "Cli"},
	{VIZ, "Visualize"}
};

void log::print(const log_source_e source, const std::string& text)
{
	cout << m_source_map[source] << ": " << text << endl;
}

void log::set_text_color(const std::string& color_sequence)
{
	std::cout << color_sequence;
}

void log::reset_text_color()
{
	std::cout << RESET;
}

void log::info(const log_source_e source, const std::string& text)
{
	set_text_color(INFO_COLOR);
	cout << "[INFO] ";
	print(source, text);
	reset_text_color();
}

void log::success(const log_source_e source, const std::string& text)
{
	set_text_color(SUCCESS_COLOR);
	cout << "[SUCCESS] ";
	print(source, text);
	reset_text_color();
}

void log::warning(const log_source_e source, const std::string& text)
{
	set_text_color(WARNING_COLOR);
	cout << "[WARNING] ";
	print(source, text);
	reset_text_color();
}

void log::error(const log_source_e source, const std::string& text)
{
	set_text_color(ERROR_COLOR);
	cout << "[ERROR] ";
	print(source, text);
	reset_text_color();
}