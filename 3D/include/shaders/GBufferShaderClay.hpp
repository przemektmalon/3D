#pragma once
#pragma once
#include "Shader.hpp"

class GBufferShaderClay : public ShaderProgram
{
public:
	GBufferShaderClay()
	{
		name.overwrite(String32("gBufferPassClay"));
		type = VertFrag;
	}

	int initialise()
	{
		use();
		projLoc = glGetUniformLocation(GLID, "proj");
		viewLoc = glGetUniformLocation(GLID, "view");
		camPosLoc = glGetUniformLocation(GLID, "camPos");
		colourLoc = glGetUniformLocation(GLID, "colour");
		roughnessLoc = glGetUniformLocation(GLID, "roughness");
		metallicLoc = glGetUniformLocation(GLID, "metallic");
		colour = glm::fvec3(0.6, 0.6, 0.6);
		roughness = 0.85;
		metallic = 0.1;
		stop();
		return 1;
	}

	void setProj(glm::fmat4& pProj)
	{
		proj = pProj;
	}

	void setView(glm::fmat4& pView)
	{
		view = pView;
	}

	void sendView()
	{
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	}

	void setCamPos(glm::fvec3& pCamPos)
	{
		camPos = pCamPos;
	}

	void sendCamPos()
	{
		glUniform3fv(camPosLoc, 1, glm::value_ptr(camPos));
	}

	void sendUniforms()
	{
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniform3fv(camPosLoc, 1, glm::value_ptr(camPos));
		glUniform3fv(colourLoc, 1, glm::value_ptr(colour));
		glUniform1f(roughnessLoc, roughness);
		glUniform1f(metallicLoc, metallic);
	}

private:
	int projLoc;
	int viewLoc;
	int camPosLoc;
	int colourLoc;
	int roughnessLoc;
	int metallicLoc;

	glm::fmat4 proj;
	glm::fmat4 view;
	glm::fvec3 camPos;
	glm::fvec3 colour;
	float roughness;
	float metallic;
};