#pragma once
#include "Shader.hpp"

class ShaderStore
{
public:
	ShaderStore() {}
	~ShaderStore() {}

	ShaderProgram* loadShader(ShaderProgram::ShaderType type, String32& name, String128& path = String128("res/shader/"))
	{
		auto itr = shaders.insert(std::make_pair(name, new ShaderProgram()));
		itr.first->second->load(name, type, path);
		itr.first->second->compile();
		itr.first->second->initialise();
		return itr.first->second;
	}

	ShaderProgram* loadShader(ShaderProgram* loc, String128& path = String128("res/shader/"))
	{
		auto itr = shaders.insert(std::make_pair(loc->getName(), loc));
		itr.first->second->load(loc->getName(), loc->getType(), path);
		itr.first->second->compile();
		itr.first->second->initialise();
		return loc;
	}

	ShaderProgram* reloadShader(String32& pName)
	{
		auto s = getShader(pName);
		if (!s)
			return nullptr;

		s->reload();
		return s;
	}

	void reloadAllShaders()
	{
		for (auto s : shaders)
		{
			s.second->reload();
		}
	}

	ShaderProgram* getShader(const String32& name)
	{
		auto find = shaders.find(name);
		if (find == shaders.end())
			return nullptr;
		else
			return find->second;
	}

	std::map<String32, ShaderProgram*>& getShaderMap()
	{
		return shaders;
	}

private:

	std::map<String32, ShaderProgram*> shaders;

};