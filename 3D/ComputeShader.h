#pragma once
#include <GL\glew.h>
//#include <string>
#include <iostream>
#include <fstream>
#include "ShaderPreprocessor.h"
#include "StringGenerics.h"

class ComputeShader : public ShaderPreprocessor
{
public:
	ComputeShader() {}
	~ComputeShader() { glDeleteProgram(program); }

	GLuint load(StringGeneric& pCompPath, bool preprocess = false)
	{
		pCompPath += String32(".comp");

		std::ifstream compStream(pCompPath.getString(), std::ifstream::in);

		if (compStream.fail())
		{
			std::cout << "Failed loading compute shader: " << std::string(pCompPath.getString()) << std::endl;
			return 0;
		}

		std::string compContent;

		compContent.assign(std::istreambuf_iterator<char>(compStream), std::istreambuf_iterator<char>());

		GLint compShader = glCreateShader(GL_COMPUTE_SHADER);

		GLchar* glCharComp = &compContent[0];

		if (preprocess)
		{
			String32 e = String32("exposure");
			String1024 v = String1024("1.f");
			setVarVal(e, v);
			glCharComp = preProcess(&compContent[0], compContent.size());
		}

		glShaderSource(compShader, 1, &glCharComp, NULL);

		if (preprocess)
		{
			delete[] glCharComp;
		}

		auto err = glGetError();
		glCompileShader(compShader);
		GLint isCompiled = GL_FALSE;
		glGetShaderiv(compShader, GL_COMPILE_STATUS, &isCompiled);
		err = glGetError();
		if (isCompiled == GL_FALSE)
		{
			//std::cout << "Failed to compile compute shader: " << pPathComp << std::endl;
		}

		compStream.close();

		auto shaderProgram = glCreateProgram();

		glAttachShader(shaderProgram, compShader);
		glLinkProgram(shaderProgram);
		glDeleteShader(compShader);

		isCompiled = GL_FALSE;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			//std::cout << "Failed to link compute shader: " << std::string(pCompPath.getString()) << std::endl;

			GLint logLength = 0;
			glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);

			//std::vector<GLchar> errorLog(maxLength);
			//GLchar* errorLog = new GLchar[logLength];
			//glGetProgramInfoLog(shaderProgram, logLength, &logLength, &errorLog[0]);

			std::vector<GLchar> errorLog(logLength);
			glGetProgramInfoLog(shaderProgram, logLength, &logLength, &errorLog[0]);

			for (int i = 0; i < logLength; ++i)
			{
				std::cout << char(errorLog[i]);
			}

			glDeleteProgram(shaderProgram);

			shaderProgram = 0;

			return 0;
		}

		//std::cout << "Compute shader program " << std::string(pCompPath.getString()) << " loaded and compiled successfully" << std::endl;

		program = shaderProgram;

		return shaderProgram;
	}

	GLint operator()()
	{
		return program;
	}

	void use() { glUseProgram(program); }
	void stop() { glUseProgram(0); }

protected:

	GLuint program;
};