#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "GL\glew.h"
#include <vector>

class Shader
{
public:
	Shader() {}
	~Shader() { glDeleteProgram(program); }

	GLuint load(std::string pPath)
	{
		std::string vertPath = pPath + ".vert";
		std::string fragPath = pPath + ".frag";

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
			std::cout << "Failed to compile fragment shader: " << pPath << std::endl;
		}

		glCompileShader(vertexShader);
		isCompiled = GL_FALSE;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			std::cout << "Failed to compile fragment shader: " << pPath << std::endl;
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
			std::cout << "Failed to link shader: " << pPath << std::endl;

			GLint maxLength = 0;
			glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> errorLog(maxLength);
			glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &errorLog[0]);

			for (int i = 0; i < errorLog.size(); ++i)
			{
				//std::cout << char(errorLog.data()[i]);
			}

			glDeleteProgram(shaderProgram);

			shaderProgram = 0;

			return 0;
		}

		std::cout << "Shader program " << pPath << " loaded and linked successfully" << std::endl;

		program = shaderProgram;

		return shaderProgram;
	}

	GLint operator()()
	{
		return program;
	}

private:

	GLuint program;
};