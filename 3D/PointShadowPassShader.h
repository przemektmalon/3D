#pragma once
#include "Shader.h"

class PointShadowPassShader : public ShaderProgram
{
public:
	PointShadowPassShader()
	{
		name.overwrite(String32("pointShadowPass"));
		type = VertGeomFrag;
	}

	int initialise()
	{
		use();
		lightPosLoc = glGetUniformLocation(GLID, "lightPos");
		farPlaneLoc = glGetUniformLocation(GLID, "farPlane");
		stop();
		return 1;
	}

	void setLightPos(glm::fvec3 pPos)
	{
		lightPos = pPos;
	}

	void setFarPlane(float pFar)
	{
		farPlane = pFar;
	}

	void sendUniforms()
	{
		glUniform3fv(lightPosLoc, 1, &lightPos[0]);
		glUniform1f(farPlaneLoc, farPlane);
	}

private:
	int lightPosLoc;
	int farPlaneLoc;

	glm::fvec3 lightPos;
	float farPlane;
	glm::fmat4 projView;
};