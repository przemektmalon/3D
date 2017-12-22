#pragma once
#include "MathIncludes.hpp"
#include "rapidxml.hpp"
#include "boost/tokenizer.hpp"

template<class T>
T xmlToVec(rapidxml::xml_attribute<char>* att)
{
	if (!att)
	{
		T ret;
		memset(&ret, 0, sizeof(ret));
		return ret;
	}

	std::string attStr;
	attStr.assign(att->value(), att->value_size());

	boost::char_separator<char> sep(" ");
	boost::tokenizer<boost::char_separator<char>> tokens(attStr, sep);

	std::vector<std::string> toks;

	for (auto& t : tokens)
		toks.push_back(t);

	T ret;
	assert(ret.length() <= toks.size());

	int i = 0;
	for (auto& t : toks)
	{
		ret[i] = std::stod(t); /// TODO: there might be problems here when converting long longs
		++i;
	}

	return ret;
}

template<class T>
T xmlToNum(rapidxml::xml_attribute<char>* att)
{
	if (!att)
	{
		T ret;
		memset(&ret, 0, sizeof(ret));
		return ret;
	}

	std::string attStr;
	attStr.assign(att->value(), att->value_size());

	T ret;

	ret = std::stod(attStr);

	return ret;
}

std::string xmlToString(rapidxml::xml_attribute<char>* att)
{
	if (!att)
		return "_NULL_";
	std::string attStr;
	attStr.assign(att->value(), att->value_size());
	return attStr;
}