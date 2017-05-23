#pragma once
#include "ConfigParser.h"

void ConfigParser::clear()
{
	data.clear();
}

std::string ConfigParser::getString(std::string section, std::string pkey)
{
	auto mapItr = data.find(std::pair<std::string, std::string>(section, pkey));

	return mapItr->second;
}
