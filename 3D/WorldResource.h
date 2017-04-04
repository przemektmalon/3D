#pragma once
#include "Types.h"
#include <Shlwapi.h>

#define ENGINE_MAX_PATH MAX_PATH

class Resource
{
public:
	Resource() {}
	~Resource() {}

	u32 getID() { return id; }
	bool isLoaded() { return loaded; }
	bool registerResource(const char* pPath)
	{
		u32 pathLength = 0;
		while (pPath != 0) {
			path[pathLength] = pPath[pathLength];
			++pathLength;
			if (pathLength + Engine::workingDirectoryLength > ENGINE_MAX_PATH) {
				//TODO: LOG ERROR
				return false;
			}
		}
		path[pathLength] = 0;
		return true;
	}

protected:
	void setLoaded(bool pLoaded)
	{
		loaded = pLoaded;
	}
	char path[ENGINE_MAX_PATH];

private:
	u32 id;
	bool loaded;
};