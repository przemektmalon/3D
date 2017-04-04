#pragma once
#include "ConfigParser.h"

void ConfigParser::parse(std::string pfilename)
{
	configFile.open(pfilename);

	int section = -1;
	int lineNum = 1;

	while (!configFile.eof())
	{
		std::string line;
		while (std::getline(configFile, line))
		{
			std::istringstream isline(line);
			std::istringstream istempline(line);
			std::string key;
			std::string value;

			if (line[0] == '[')
			{
				if (std::getline(isline, key, '='))
				{
					if (std::getline(isline, value, ']'))
					{
						section = std::stoi(value);
					}
				}
			}
			else
			{
				if (std::getline(isline, key, '='))
				{
					if (std::getline(isline, value))
					{
						//tempMap.insert(std::pair<std::string,std::string>(key,value));
						//tempMap2.insert(std::pair<int,int>(lineNum,section));
						data.insert(std::pair<std::pair<int, std::string>, std::pair<int, std::string>>(std::pair<int, std::string>(section, key), std::pair<int, std::string>(lineNum, value)));
					}
				}
			}
			++lineNum;
		}
	}
	configFile.close();
}

void ConfigParser::clear()
{
	data.clear();
}

std::string ConfigParser::getValue(int section, std::string pkey)
{
	//std::map<std::map<int,int>,std::map<std::string,std::string>>::iterator mapItr = data.find(section);
	//std::map<int,std::map<std::string,std::string>>::iterator mapItr = data.find(section);
	//std::map<std::string,std::string> subMap = mapItr->second;
	//std::map<std::string,std::string>::iterator subMapItr = subMap.find(pkey);

	auto mapItr = data.find(std::pair<int, std::string>(section, pkey));

	return mapItr->second.second;
}

void ConfigParser::printAll()
{
	//std::map<std::pair<int,int>,std::map<std::string,std::string>>::iterator mapItr = data.begin();
	//std::map<std::pair<int,std::string>,std::string>::iterator mapItr = data.begin();
	//std::map<std::string,std::string> tempMap = mapItr->second;
	//std::map<std::string,std::string>::iterator subMapItr = tempMap.begin();

	auto mapItr = data.begin();

	std::cout << data.size() << std::endl;

	while (mapItr != data.end())
	{
		std::cout << "S:" << mapItr->first.first << " - " << mapItr->first.second << "=" << mapItr->second.second << " Line:" << mapItr->second.first << std::endl;
		++mapItr;
	}
}

int ConfigParser::getSize()
{
	return data.size();
}
