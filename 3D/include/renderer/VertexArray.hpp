#pragma once
#include "GLIncludes.hpp"
#include "shaders\Shader.hpp"
#include <string>
#include <map>

class VAO
{
public:
	void create()
	{
		glGenVertexArrays(1, &GLID);
	}

	void addAttrib(std::string pName, GLint pSize, GLenum pType)
	{
		if (attribs.find(pName) != attribs.end())
			return; /// TODO: log

		attribs.insert(std::make_pair(pName, Attrib(pName, pSize, pType)));
		totalSize += pSize;
	}

	void enableFor(ShaderProgram& shader)
	{
		bind();
		shader.use();

		int offset = 0;

		for (auto& att : attribs)
		{
			GLint pos = glGetAttribLocation(shader.getGLID(), att.second.name.c_str());
			glEnableVertexAttribArray(pos);
			glVertexAttribPointer(pos, att.second.size, att.second.type, GL_FALSE, totalSize * sizeof(GLfloat), (void*)((offset * sizeof(GLfloat))));
			offset += att.second.size;
		}
	}

	void bind()
	{
		glBindVertexArray(GLID);
	}

	void unbind()
	{
		glBindVertexArray(0);
	}

private:

	struct Attrib
	{
		Attrib(std::string pName, GLint pSize, GLenum pType) : name(pName), size(pSize), type(pType) {}
		std::string name;
		GLint size;
		GLenum type;
	};

	std::map<std::string,Attrib> attribs;
	int totalSize;
	GLuint GLID;

};