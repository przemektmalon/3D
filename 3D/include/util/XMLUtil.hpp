#pragma once
#include "MathIncludes.hpp"
#include "rapidxml.hpp"
#include "boost/tokenizer.hpp"
#include "boost/algorithm/string.hpp"

#include "File.hpp"
#include <string>
#include <map>

using namespace rapidxml;

class XML
{
public:
	std::string xmlString;
	rapidxml::xml_document<> doc;

public:

	void parseFile(std::string&& path) { parseFile(path); }
	void parseFile(std::string& path)
	{
		File xmlFile;
		xmlFile.open(path, File::in);

		if (!xmlFile.fstream().good())
			assert(false); /// TODO: handle error

		xmlFile.fstream().seekg(0, std::ios::end);
		xmlString.reserve(xmlFile.fstream().tellg());
		xmlFile.fstream().seekg(0, std::ios::beg);

		xmlString.assign((std::istreambuf_iterator<char>(xmlFile.fstream())), std::istreambuf_iterator<char>());

		doc.parse<0>((char*)xmlString.c_str());
		xmlFile.close();
	}

	rapidxml::xml_node<>* getNode(rapidxml::xml_node<>* parent, std::string&& name) { return getNode(parent, name); }
	rapidxml::xml_node<>* getNode(rapidxml::xml_node<>* parent, std::string& name)
	{
		return parent->first_node(name.c_str());
	}

	rapidxml::xml_node<>* next(rapidxml::xml_node<>* cur, std::string&& name) { return next(cur, name); }
	rapidxml::xml_node<>* next(rapidxml::xml_node<>* cur, std::string& name) 
	{
		return cur->next_sibling(name.c_str());
	}

	rapidxml::xml_attribute<>* getAtt(rapidxml::xml_node<>* node, std::string&& name) { return getAtt(node, name); }
	rapidxml::xml_attribute<>* getAtt(rapidxml::xml_node<>* node, std::string& name)
	{
		return node->first_attribute(name.c_str());
	}

	void tokenize(std::string str, std::vector<std::string>& retTokens)
	{
		boost::char_separator<char> sep(" ,");
		boost::tokenizer<boost::char_separator<char>> tokens(str, sep);

		for (auto& t : tokens)
			retTokens.push_back(t);
	}

	template<class T>
	T toVec(rapidxml::xml_base<>* att)
	{
		if (!att)
		{
			T ret;
			memset(&ret, 0, sizeof(ret));
			return ret;
		}

		std::string valueStr;
		valueStr.assign(att->value(), att->value_size());

		std::vector<std::string> tokens; tokenize(valueStr, tokens);

		T ret;
		assert(ret.length() <= tokens.size());

		int i = 0;
		for (auto& t : tokens)
		{
			ret[i] = std::stod(t); /// TODO: there might be problems here when converting long longs
			++i;
		}

		return ret;
	}

	template<class T>
	T toVal(char* begin);
	template<>
	float toVal<float>(char* begin) { return std::stof(begin); }
	template<>
	int toVal<int>(char* begin) { return std::stoi(begin); }
	
	template<class T>
	T toVal(rapidxml::xml_base<>* base);
	template<>
	float toVal<float>(rapidxml::xml_base<>* base) { return toVal<float>((char*)getString(base).c_str()); }
	template<>
	int toVal<int>(rapidxml::xml_base<>* base) { return toVal<float>((char*)getString(base).c_str()); }

	void getString(rapidxml::xml_base<>* base, std::string& ret)
	{
		ret.assign(base->value(), base->value_size());
	}

	std::string getString(rapidxml::xml_base<>* base)
	{
		if (!base)
			return "_NULL_";
		return std::string(base->value(), base->value_size());
	}

};