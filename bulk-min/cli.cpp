#include "cli.h"

using namespace std;

void cli::parse_and_execute_command(const std::string& input)
{
	istringstream iss(input);
	string commandName;
	iss >> commandName;

	map<string, string> options;
	string option_pair;

	while (iss >> option_pair)
	{
		size_t equal_pos = option_pair.find('=');

		if (equal_pos != string::npos)
		{
			string key = option_pair.substr(0, equal_pos);
			string value = option_pair.substr(equal_pos + 1);
			options[key] = value;
		}
		else
		{
			log::error(CLI, "invalid option format: " + option_pair);

			return;
		}
	}

	execute_command(commandName, options);
}

void cli::execute_command(const std::string& name, std::map<std::string, std::string>& options)
{
	auto it = m_commands.find(name);
	if (it != m_commands.end())
	{
		it->second(options);
	}
	else
	{
		log::error(CLI, "command not found: " + name);
	}
}

void cli::register_command(const std::string& command, const std::function<void(const std::map<std::string, std::string>&)>& callback)
{
	m_commands[command] = callback;
}

void cli::start()
{
	string input;

	while (true)
	{
		getline(cin, input);

		if (input == "exit")
		{
			break;
		}

		parse_and_execute_command(input);
	}
}

bool cli::require_option(const std::map<std::string, std::string> options, const std::string& name, std::string& value) const
{
	if (match_option(options, name, value))
	{
		return true;
	}
	
	log::error(CLI, "expected option: " + name);

	return false;
}

bool cli::match_option(const std::map<std::string, std::string> options, const std::string& name, std::string& value) const
{
	auto it = options.find(name);

	if (it != options.end())
	{
		value = it->second;
		return true;
	}

	return false;
}
