#pragma once
#include "Shader.h"
#include <map>

class SAOShader : public ShaderProgram
{
public:
	SAOShader() {}

	int initialise()
	{
		load("ssao", VertFrag);
		compile();
		use();
		projScaleLoc = glGetUniformLocation(GLID, "projScale");
		depthBufferLoc = glGetUniformLocation(GLID, "depthBuffer");
		radiusLoc = glGetUniformLocation(GLID, "radius");
		biasLoc = glGetUniformLocation(GLID, "bias");
		intensityDivR6Loc = glGetUniformLocation(GLID, "intensityDivR6");
		gNormalLoc = glGetUniformLocation(GLID, "gNormal");
		viewLoc = glGetUniformLocation(GLID, "view");
		projInfoLoc = glGetUniformLocation(GLID, "projInfo");

		setRadius(0.36f);
		setIntensity(2.4f);

		stop();

		return 1;
	}

	void setProj(glm::fmat4 &proj, glm::ivec2 viewport)
	{
		use();
		float scale = 500.f;
		glUniform1f(projScaleLoc, scale);
		glm::fvec4 projInfo(-2.f / (viewport.x * proj[0][0]), -2.f / (viewport.y*proj[1][1]), (1.f - proj[0][2]) / proj[0][0], (1.f + proj[1][2]) / proj[1][1]);
		glUniform4fv(projInfoLoc, 1, glm::value_ptr(projInfo));
		//TODO: set projScale
	}

	void setRadius(float pRadius)
	{
		use();
		radius = pRadius;
		glUniform1f(radiusLoc, radius);
		glUniform1f(intensityDivR6Loc, intensity / std::powf(radius, 6.f));
	}

	void setIntensity(float pIntensity)
	{
		use();
		intensity = pIntensity;
		glUniform1f(intensityDivR6Loc, intensity / std::powf(radius, 6.f));
	}


	//Variables
	float radius;
	float bias;
	float intensity;

private:
	//Locations
	int projScaleLoc;
	int depthBufferLoc;
	int radiusLoc;
	int biasLoc;
	int intensityDivR6Loc;
	int gNormalLoc;
	int viewLoc;
	int projInfoLoc;

};