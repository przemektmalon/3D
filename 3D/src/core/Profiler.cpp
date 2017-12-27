#include "Profiler.hpp"
#include "Engine.hpp"
#include "QPC.hpp"
#include <gl\glew.h>
#include <gl\GL.h>

void Profiler::start(std::string id)
{
	times[id].first.setMicroSeconds(Engine::qpc.now());
}

void Profiler::end(std::string id)
{
	times[id].second.setMicroSeconds(Engine::qpc.now() - times[id].first.getMicroSeconds());
}

void Profiler::glEnd(std::string id)
{
#if ENABLE_GL_PROFILING
	glFinish();
#endif
	times[id].second.setMicroSeconds(Engine::qpc.now() - times[id].first.getMicroSeconds());
}