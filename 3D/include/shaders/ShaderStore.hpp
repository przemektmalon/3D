#pragma once
#include "Shader.hpp"

class ShaderStore
{
public:
	ShaderStore() {}
	~ShaderStore() {}

	ShaderProgram* loadShader(ShaderProgram::ShaderType type, std::string&& name, std::string&& path = std::string("res/shader/"))
	{
		return loadShader(type, name, path);
	}

	ShaderProgram* loadShader(ShaderProgram::ShaderType type, std::string& name, std::string& path = std::string("res/shader/"))
	{
		auto itr = shaders.insert(std::make_pair(name, new ShaderProgram()));
		itr.first->second->load(name, type, path);
		itr.first->second->compile();
		itr.first->second->initialise();
		return itr.first->second;
	}

	ShaderProgram* loadShader(ShaderProgram* loc, std::string& path = std::string("res/shader/"))
	{
		auto itr = shaders.insert(std::make_pair(loc->getName(), loc));
		itr.first->second->load(loc->getName(), loc->getType(), path);
		itr.first->second->compile();
		itr.first->second->initialise();
		return loc;
	}

	ShaderProgram* reloadShader(std::string& pName)
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

	ShaderProgram* getShader(const std::string& name)
	{
		auto find = shaders.find(name);
		if (find == shaders.end())
			return nullptr;
		else
			return find->second;
	}

	std::map<std::string, ShaderProgram*>& getShaderMap()
	{
		return shaders;
	}

private:

	std::map<std::string, ShaderProgram*> shaders;

};