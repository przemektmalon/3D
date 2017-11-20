#pragma once
#include "Shader.hpp"
#include "Include.hpp"

class DeferredTileCullComputeShader : public ShaderProgram
{
public:
	DeferredTileCullComputeShader() 
	{
		name.overwrite(String32("pbrshader")); 
		type = Compute;

	}
	~DeferredTileCullComputeShader() {}

	int initialise()
	{
		use();
		viewPosLoc = glGetUniformLocation(GLID, "viewPos");
		viewRaysLoc = glGetUniformLocation(GLID, "viewRays");
		projLoc = glGetUniformLocation(GLID, "proj");
		viewLoc = glGetUniformLocation(GLID, "view");
		exposureLoc = glGetUniformLocation(GLID, "exposure");
		pointLightCountLoc = glGetUniformLocation(GLID, "pointLightCount");
		spotLightCountLoc = glGetUniformLocation(GLID, "spotLightCount");
		selectedIDLoc = glGetUniformLocation(GLID, "selectedID");
		renderModeLoc = glGetUniformLocation(GLID, "renderMode");
		setExposure(3.f);
		setRenderMode(0);
		sendUniforms();
		stop();
		return 1;
	}

	void setSelectedID(u32 pID)
	{
		selectedID = pID;
	}

	void sendSelectedID()
	{
		glUniform1i(selectedIDLoc, selectedID);
	}

	void setViewPos(glm::fvec3& pViewPos)
	{
		viewPos = pViewPos;
	}

	void sendViewPos()
	{
		glUniform3fv(viewPosLoc, 1, &viewPos.x);
	}

	void setViewRays(glm::fvec4& pViewRays)
	{
		viewRays = pViewRays;
	}

	void sendViewRays()
	{
		glUniform4fv(viewRaysLoc, 1, &viewRays.x);
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
		glUniformMatrix4fv(viewLoc, 1, GL_TRUE, glm::value_ptr(view));
	}

	void setExposure(float pExposure)
	{
		exposure = pExposure;
	}

	void setPointLightCount(int pPointLightCount)
	{
		pointLightCount = pPointLightCount;
	}

	void setSpotLightCount(int pSpotLightCount)
	{
		spotLightCount = pSpotLightCount;
	}

	void setRenderMode(int pRenderMode)
	{
		renderMode = pRenderMode;
	}

	void sendUniforms()
	{
		glUniform1ui(selectedIDLoc, selectedID);
		glUniform3fv(viewPosLoc, 1, &viewPos.x);
		glUniform4fv(viewRaysLoc, 1, &viewRays.x);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(viewLoc, 1, GL_TRUE, glm::value_ptr(view));
		glUniform1f(exposureLoc, exposure);
		glUniform1ui(pointLightCountLoc, pointLightCount);
		glUniform1ui(spotLightCountLoc, spotLightCount);
		glUniform1i(renderModeLoc, renderMode);
	}

private:

	int viewPosLoc;
	int viewRaysLoc;
	int projLoc;
	int viewLoc;
	int exposureLoc;
	int pointLightCountLoc;
	int spotLightCountLoc;
	int selectedIDLoc;
	int renderModeLoc;

	glm::fvec3 viewPos;
	glm::fvec4 viewRays;
	glm::fmat4 proj;
	glm::fmat4 view;
	float exposure;
	int pointLightCount;
	int spotLightCount;
	int selectedID;
	int renderMode;

};