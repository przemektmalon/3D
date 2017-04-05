#pragma once
#include "Shader.h"

class FrustCullShader : public ShaderProgram
{
public:
	FrustCullShader()
	{
		name.overwrite(String32("frustCull"));
		type = Compute;
	}
	~FrustCullShader() {}

	int initialise()
	{
		use();
		projLoc = glGetUniformLocation(GLID, "proj");
		viewLoc = glGetUniformLocation(GLID, "view");
		planesLoc = glGetUniformLocation(GLID, "planes");
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

	void sendPlanes()
	{
		glUniformMatrix4fv(planesLoc, 1, GL_FALSE, &(planes[0][0]));
	}

	void sendUniforms()
	{
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(planesLoc, 1, GL_FALSE, glm::value_ptr(planes));
	}

private:

	//int objectCountLoc;
	int projLoc;
	int viewLoc;
	int planesLoc;

	//u32 objectCount;
	glm::fmat4 proj;
	glm::fmat4 view;
	glm::fmat4 planes;

};