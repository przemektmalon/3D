#pragma once
#include "Shader.h"

class SpotShadowPassShader : public ShaderProgram
{
public:
	SpotShadowPassShader()
	{
		name.overwrite(String32("spotShadowPass"));
		type = VertFrag;
	}

	int initialise()
	{
		use();
		projViewLoc = glGetUniformLocation(GLID, "projView");
		stop();
		return 1;
	}

	void setProj(glm::fmat4 pProj)
	{
		proj = pProj;
		projView = proj * view;
	}

	void setView(glm::fmat4 pView)
	{
		view = pView;
		projView = proj * view;
	}

	void sendUniforms()
	{
		glUniformMatrix4fv(projViewLoc, 1, GL_FALSE, &projView[0][0]);
	}

private:
	int projViewLoc;
	int viewLoc;

	glm::fmat4 proj;
	glm::fmat4 view;
	glm::fmat4 projView;
};