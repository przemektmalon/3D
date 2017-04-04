#pragma once
#include "ComputeShader.h"
#include "Include.h"

class DeferredTileCullComputeShader : public ShaderProgram
{
public:
	DeferredTileCullComputeShader() 
	{
		name.overwrite(String32("tileCull")); 
		type = Compute;

	}
	~DeferredTileCullComputeShader() {}

	int initialise()
	{
		//load(String32("res/shader/tileCull"), true);
		//load("tileCull", Compute);
		//compile();
		use();
		viewPosLoc = glGetUniformLocation(GLID, "viewPos");
		viewRaysLoc = glGetUniformLocation(GLID, "viewRays");
		projLoc = glGetUniformLocation(GLID, "proj");
		viewLoc = glGetUniformLocation(GLID, "view");
		exposureLoc = glGetUniformLocation(GLID, "exposure");
		pointLightCountLoc = glGetUniformLocation(GLID, "pointLightCount");
		spotLightCountLoc = glGetUniformLocation(GLID, "spotLightCount");
		selectedIDLoc = glGetUniformLocation(GLID, "selectedID");
		//setExposure(11.f);
		setPointLightCount(100);
		setSpotLightCount(0);
		stop();
		return 1;
	}

	void setSelectedID(u32 pID)
	{
		glUniform1ui(selectedIDLoc, pID);
	}

	void setViewPos(glm::fvec3& viewPos)
	{
		glUniform3fv(viewPosLoc, 1, &viewPos.x);
	}

	void setViewRays(glm::fvec4& viewRays)
	{
		glUniform4fv(viewRaysLoc, 1, &viewRays.x);
	}

	void setProj(glm::fmat4& proj)
	{
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
	}

	void setView(glm::fmat4& view)
	{
		glUniformMatrix4fv(viewLoc, 1, GL_TRUE, glm::value_ptr(view));
	}

	void setExposure(float exposure)
	{
		glUniform1f(exposureLoc, exposure);
	}

	void setPointLightCount(int pointLightCount)
	{
		glUniform1ui(pointLightCountLoc, pointLightCount);
	}

	void setSpotLightCount(int spotLightCount)
	{
		glUniform1ui(spotLightCountLoc, spotLightCount);
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

};