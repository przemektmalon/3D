#pragma once
#include "Shader.h"

class GBufferShaderMultiTex : public ShaderProgram
{
public:
	GBufferShaderMultiTex()
	{
		name.overwrite(String32("gBufferPassMultiTex"));
		type = VertFrag;
	}

	int initialise()
	{
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
		diffuse = pBinding;
	}

	void setSpecularBinding(int pBinding)
	{
		specular = pBinding;
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

	void setModel(glm::fmat4& pModel)
	{
		model = pModel;
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
		glUniform1i(diffuseLoc, diffuse);
		glUniform1i(specularLoc, specular);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1ui(IDLoc, id);
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

	int diffuse;
	int specular;
	glm::fmat4 proj;
	glm::fmat4 view;
	glm::fmat4 model;
	u32 id;
	glm::fvec3 camPos;

};