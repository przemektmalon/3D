#pragma once
#include "Shader.h"

class GBufferShader : public ShaderProgram
{
public:
	GBufferShader() {}

	int initialise()
	{
		load("gBufferPass", VertFrag);
		compile();
		use();
		diffuseLoc = glGetUniformLocation(GLID, "diffuse");
		specularLoc = glGetUniformLocation(GLID, "specular");
		projLoc = glGetUniformLocation(GLID, "proj");
		viewLoc = glGetUniformLocation(GLID, "view");
		modelLoc = glGetUniformLocation(GLID, "model");
		IDLoc = glGetUniformLocation(GLID, "id");
		camPosLoc = glGetUniformLocation(GLID, "camPos");
		setDiffuseBinding(0);
		setSpecularBinding(1);
		stop();
		return 1;
	}

	void setDiffuseBinding(int pBinding)
	{
		use();
		glUniform1i(diffuseLoc, pBinding);
	}

	void setSpecularBinding(int pBinding)
	{
		use();
		glUniform1i(specularLoc, pBinding);
	}

	void setProj(glm::fmat4& proj)
	{
		use();
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
	}

	void setView(glm::fmat4& view)
	{
		use();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	}

	void setModel(glm::fmat4& model)
	{
		use();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	}

	void setID(u32 id)
	{
		use();
		glUniform1ui(IDLoc, id);
	}

	void setCamPos(glm::fvec3& camPos)
	{
		use();
		glUniform3fv(camPosLoc, 1, glm::value_ptr(camPos));
	}

private:
	int diffuseLoc;
	int specularLoc;
	int projLoc;
	int viewLoc;
	int modelLoc;
	int IDLoc;
	int camPosLoc;
};