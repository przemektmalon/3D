#pragma once
#include "GBufferShaderBase.hpp"

class GBufferShaderNonBindlessRegular : public GBufferShaderBase
{
public:
	GBufferShaderNonBindlessRegular()
	{
		name = "gBufferPassNonBindlessRegular";
		type = VertFrag;
	}

	int initialise()
	{
		GBufferShaderBase::initialise();
		use();
		auto taloc = glGetUniformLocation(GLID, "textureArray");
		auto tloc = glGetUniformLocation(GLID, "tex");
		glUniform1i(taloc, 6);
		glUniform1i(tloc, 0);
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

	void setID(u32 pID)
	{
		id = pID;
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
		glUniform1ui(IDLoc, id);
		glUniform3fv(camPosLoc, 1, glm::value_ptr(camPos));
	}

	int texArrLoc;


private:

};