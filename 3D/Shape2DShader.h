#pragma once
#include "Shader.h"

class Shape2DShader : public ShaderProgram
{
public:
	Shape2DShader() {}
	~Shape2DShader() {}

	int initialise()
	{
		//load("res/shader/Shape2DShader", "res/shader/Shape2DShader");
		load(String32("Shape2DShader"), ShaderProgram::VertFrag);
		compile();
		use();
		projModelLoc = glGetUniformLocation(getGLID(), "projModel");
		stop();
		return 1;
	}

	void setProjModel(glm::fmat4& projModel)
	{
		use();
		glUniformMatrix4fv(projModelLoc, 1, GL_FALSE, glm::value_ptr(projModel));
	}


private:
	int projModelLoc;

};

