#pragma once
#include <string>
#include <unordered_map>
#include "Time.hpp"
#include <functional>

#define GL_PROFILE_THIS(func,name) Engine::profiler.start(name); func; Engine::profiler.glEnd(name);
#define GL_PROFILE_START(name) Engine::profiler.start(name);
#define GL_PROFILE_END(name) Engine::profiler.glEnd(name)

#define PROFILE_THIS(func,name) Engine::profiler.start(name); func; Engine::profiler.end(name);
#define PROFILE_START(name) Engine::profiler.start(name);
#define PROFILE_END(name) Engine::profiler.end(name)

#define PROFILE_TIME(name) Engine::profiler.getTime(name)

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

private:

	std::unordered_map<std::string, std::pair<Time,Time>> times;

};