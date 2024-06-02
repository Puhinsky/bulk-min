#include "simplex.h"
#include "parser.h"
#include <memory>
#include "cli.h"
#include "exporter.h"

using namespace std;

//..\\input_files\\test\\BULK_5.txt

void on_parse(const map<string, string>& options);
void on_export(const map<string, string>& options);
void on_cycle(const map<string, string>& options);
void on_genetic(const map<string, string>& options);
void on_simplex(const map<string, string>& options);

cli g_cli;
shared_ptr<database> g_data;
parser g_parser;
exporter g_exporter;
simplex g_simplex;

int main()
{
	g_cli.register_command("parse",on_parse);
	g_cli.register_command("export", on_export);
	g_cli.register_command("cycle", on_cycle);
	g_cli.register_command("genetic", on_genetic);
	g_cli.register_command("simplex", on_simplex);

	g_cli.start();

	return 0;
}

void on_parse(const map<string, string>& options)
{
	string file_path;

	if (g_cli.require_option(options, "file", file_path))
	{
		g_data = make_shared<database>(g_parser.load(file_path));
	}
}

void on_export(const map<string, string>& options)
{
	string file_path;

	if (g_cli.require_option(options, "file", file_path))
	{
		g_exporter.export_data(g_data.get(), file_path);
	}
}

void on_cycle(const map<string, string>&options)
{}

void on_genetic(const map<string, string>&options)
{}

void on_simplex(const map<string, string>& options)
{
	string parameter;

	if (g_cli.match_option(options, "step", parameter))
	{
		g_simplex.set_step(stod(parameter));
	}

	if (g_cli.match_option(options, "min_step", parameter))
	{
		g_simplex.set_min_step(stod(parameter));
	}

	if (g_cli.match_option(options, "step_div", parameter))
	{
		g_simplex.set_step_divider(stod(parameter));
	}

	if (g_cli.match_option(options, "round", parameter))
	{
		g_simplex.set_round_enable(stoi(parameter));
	}

	g_simplex.run(g_data);
}