#pragma once
//#include "Include.h"
#include "Types.h"
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

class ConfigParser
{
public:
	ConfigParser() {}
	~ConfigParser() {}

	void parse(std::string filename)
	{
		file.open(filename);

		std::string section;

		while (!file.eof())
		{
			std::string line;
			while (std::getline(file, line))
			{
				if (line.length() < 3)
					continue;

				std::istringstream isline(line);
				std::istringstream istempline(line);
				std::string key;
				std::string value;

				if (line[0] == '[')
				{
					if (std::getline(isline, key, ']'))
					{
						section = key;
					}
				}
				else
				{
					if (std::getline(isline, key, '='))
					{
						if (std::getline(isline, value))
						{
							data.insert(std::make_pair(std::make_pair(section, key),value));
						}
					}
				}
			}
		}
		file.close();
	}
	void clear();

	s64 getInteger(std::string section, std::string key);
	double getFloating(std::string section, std::string key);
	std::string getString(std::string section, std::string key);

private:

	std::ifstream file;
	std::map<std::pair<std::string, std::string>, std::string> data;
};