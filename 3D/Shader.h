#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "GL\glew.h"
#include <vector>
#include <glm\glm.hpp>
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\matrix_transform_2d.hpp"

class Shader
{
public:
	Shader() {}
	~Shader() { glDeleteProgram(program); }

	GLuint load(std::string pPathVert, std::string pPathFrag)
	{
		std::string vertPath = pPathVert + ".vert";
		std::string fragPath = pPathFrag + ".frag";

		std::ifstream vertStream(vertPath, std::ifstream::in);
		std::ifstream fragStream(fragPath, std::ifstream::in);

		if (vertStream.fail())
		{
			std::cout << "Failed loading vertex shader: " << vertPath << std::endl;
			return 0;
		}

		if (fragStream.fail())
		{
			std::cout << "Failed loading fragment shader: " << fragPath << std::endl;
			return 0;
		}

		std::string fragContent;
		std::string vertContent;

		fragContent.assign(std::istreambuf_iterator<char>(fragStream), std::istreambuf_iterator<char>());
		vertContent.assign(std::istreambuf_iterator<char>(vertStream), std::istreambuf_iterator<char>());

		GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		const GLchar* const glCharFrag = fragContent.c_str();
		const GLchar* const glCharVert = vertContent.c_str();

		glShaderSource(fragmentShader, 1, &glCharFrag, NULL);
		glShaderSource(vertexShader, 1, &glCharVert, NULL);

		auto err = glGetError();
		glCompileShader(fragmentShader);
		GLint isCompiled = GL_FALSE;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		err = glGetError();
		if (isCompiled == GL_FALSE)
		{
			std::cout << "Failed to compile fragment shader: " << pPathFrag << std::endl;
		}

		glCompileShader(vertexShader);
		isCompiled = GL_FALSE;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			std::cout << "Failed to compile vertex shader: " << pPathVert << std::endl;
		}

		fragStream.close();
		vertStream.close();

		auto shaderProgram = glCreateProgram();

		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glBindFragDataLocation(shaderProgram, 0, "outColour");
		glLinkProgram(shaderProgram);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		isCompiled = GL_FALSE;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			std::cout << "Failed to link shader: " << pPathFrag << " + " << pPathVert << std::endl;

			GLint maxLength = 0;
			glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> errorLog(maxLength);
			glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &errorLog[0]);

			for (int i = 0; i < errorLog.size(); ++i)
			{
				std::cout << char(errorLog.data()[i]);
			}

			glDeleteProgram(shaderProgram);

			shaderProgram = 0;

			return 0;
		}

		std::cout << "Shader program " << pPathFrag << " + " << pPathVert << " loaded and linked successfully" << std::endl;

		program = shaderProgram;

		return shaderProgram;
	}

	virtual void initialise() {}

	GLint operator()()
	{
		return program;
	}

	void use() { glUseProgram(program); }
	void stop() { glUseProgram(0); }

protected:

	GLuint program;
};

class StandardShader : public Shader
{
public:
	StandardShader() {
		load("Standard", "Standard");

	}
	~StandardShader() {}

	enum Uniform
	{
		viewPos = 0,
		diffuse = 1,
		specular = 2,
		emit = 3,
		emitStrength = 4,
		shininess = 5,
		proj = 6,
		model = 7
	};

	void setViewPos(glm::fvec3 pos)
	{
		glUniform3f(Uniform::viewPos, pos.x, pos.y, pos.z);
	}

	void setDiffuse(GLuint texUnit)
	{
		glUniform1i(Uniform::diffuse, texUnit);
	}
	void setSpecular(GLuint texUnit)
	{
		glUniform1i(Uniform::specular, texUnit);
	}
	void setEmit(GLuint texUnit)
	{
		glUniform1i(Uniform::emit, texUnit);
	}

	void setEmitStrength(float str)
	{
		glUniform1f(Uniform::emitStrength, str);
	}

	void setShininess(float shin)
	{
		glUniform1f(Uniform::shininess, shin);
	}

	void setProjMatrix(glm::fmat4 proj)
	{
		glUniformMatrix4fv(Uniform::proj, 1, GL_FALSE, glm::value_ptr(proj));
	}

	void setModelMatric(glm::fmat4 model)
	{
		glUniformMatrix4fv(Uniform::model, 1, GL_FALSE, glm::value_ptr(model));
	}
};