#pragma once
//#include "Include.h"
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

	void parse(std::string filename);
	void clear();
	std::string getValue(int section, std::string key);
	int getSize();
	void printAll();

private:
	std::ifstream configFile;
	//std::map<int,std::map<std::string,std::string>> data;
	std::map<std::pair<int, std::string>, std::pair<int, std::string>> data;	//FIRST PAIR == SECTION -> KEY
																				//SECOND PAIR == LINE -> VALUE
};