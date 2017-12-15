#pragma once
#include "Shader.hpp"

class GBufferShaderTex : public ShaderProgram
{
public:
	GBufferShaderTex() 
	{
		name.overwrite(String32("gBufferPassTex"));
		type = VertFrag;
	}

	int initialise()
	{
		use();
		projLoc = glGetUniformLocation(GLID, "proj");
		viewLoc = glGetUniformLocation(GLID, "view");
		camPosLoc = glGetUniformLocation(GLID, "camPos");
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
	}

private:
	int projLoc;
	int viewLoc;
	int camPosLoc;

	glm::fmat4 proj;
	glm::fmat4 view;
	glm::fvec3 camPos;
};