#pragma once
#include <string>
#include <unordered_map>
#include "Time.hpp"
#include <functional>

class Profiler
{
public:
	Profiler() {}
	~Profiler() {}

	void start(std::string id);
	void end(std::string id);
	void glEnd(std::string id);

	Time getTime(std::string id)
	{
		return times[id].second;
	}

	void glTimeThis(std::function<void(void)> func, std::string id)
	{
		start(id);
		func();
		glEnd(id);
	}

	void timeThis(std::function<void(void)> func, std::string id)
	{
		start(id);
		func();
		end(id);
	}

private:

	std::unordered_map<std::string, std::pair<Time,Time>> times;

};