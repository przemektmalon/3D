#pragma once
#include "Shader.hpp"

class Shape2DShaderNoTex : public ShaderProgram
{
public:
	Shape2DShaderNoTex()
	{
		name.overwrite(String32("Shape2DShaderNoTex"));
		type = VertFrag;
	}
	~Shape2DShaderNoTex() {}

	int initialise()
	{
		//load("res/shader/Shape2DShader", "res/shader/Shape2DShader");
		load(String32("Shape2DShaderNoTex"), ShaderProgram::VertFrag);
		compile();
		use();
		projModelLoc = glGetUniformLocation(getGLID(), "projModel");
		colourLoc = glGetUniformLocation(getGLID(), "colour");
		stop();
		return 1;
	}

	void setProjModel(glm::fmat4& pProjModel)
	{
		projModel = pProjModel;
	}

	void setColour(glm::fvec4& pColour)
	{
		colour = pColour;
	}

	void sendUniforms()
	{
		glUniformMatrix4fv(projModelLoc, 1, GL_FALSE, glm::value_ptr(projModel));
		glUniform4fv(colourLoc, 1, &colour[0]);
	}

private:
	int projModelLoc;
	int colourLoc;

	glm::fmat4 projModel;
	glm::fvec4 colour;
};
