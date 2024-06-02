#pragma once

#include "log.h"
#include <string>
#include <map>
#include <functional>
#include <iostream>

class cli
{
private:
	std::map<std::string, std::function<void(const std::map<std::string, std::string>&)>> m_commands;
	void parse_and_execute_command(const std::string& input);
	void execute_command(const std::string& name, std::map<std::string, std::string>& options);

public:
	void register_command(const std::string& command, const std::function<void(const std::map<std::string, std::string>&)>& callback);
	void start();
	bool require_option(const std::map<std::string, std::string> options, const std::string& name, std::string& value) const;
	bool match_option(const std::map<std::string, std::string> options, const std::string& name, std::string& value) const;
};

