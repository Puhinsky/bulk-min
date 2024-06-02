#include "cli.h"

using namespace std;

void cli::parse_and_execute_command(const std::string& input)
{
	size_t spacePos = input.find(' ');
	string commandName = input.substr(0, spacePos);
	string optionsString = (spacePos != string::npos) ? input.substr(spacePos + 1) : "";

	map<string, string> options;
	size_t prevPos = 0;
	size_t equalPos;

	while ((equalPos = optionsString.find('=', prevPos)) != string::npos)
	{
		string key = optionsString.substr(prevPos, equalPos - prevPos);
		prevPos = equalPos + 1;
		size_t nextPos = optionsString.find(' ', prevPos);
		string value = optionsString.substr(prevPos, nextPos - prevPos);
		options[key] = value;
		prevPos = nextPos + 1;
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
