#pragma once
#include "Shader.hpp"

class PrepMultiTexShader : public ShaderProgram
{
public:
	PrepMultiTexShader()
	{
		name = "prepMultiTex";
		type = Compute;
	}
	~PrepMultiTexShader() {}

	int initialise()
	{
		use();
		projLoc = glGetUniformLocation(GLID, "proj");
		viewLoc = glGetUniformLocation(GLID, "view");
		planesLoc = glGetUniformLocation(GLID, "planes");
		baseIDLoc = glGetUniformLocation(GLID, "baseID");
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

	void setPlanes(glm::fmat4& pPlanes)
	{
		planes = pPlanes;
	}

	void setBaseID(u32 pBaseID)
	{
		baseID = pBaseID;
	}

	void sendPlanes()
	{
		glUniformMatrix4fv(planesLoc, 1, GL_FALSE, &(planes[0][0]));
	}

	void sendUniforms()
	{
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(planesLoc, 1, GL_FALSE, glm::value_ptr(planes));
		glUniform1ui(baseIDLoc, baseID);
	}

private:

	//int objectCountLoc;
	int projLoc;
	int viewLoc;
	int planesLoc;
	int baseIDLoc;

	//u32 objectCount;
	glm::fmat4 proj;
	glm::fmat4 view;
	glm::fmat4 planes;
	u32 baseID;

};