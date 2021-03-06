#pragma once
#include "Shader.hpp"

class BilateralBlurShader : public ShaderProgram
{
private:
	int axisLoc;
	int sourceLoc;

	glm::ivec2 axis;
	int source;

public:
	BilateralBlurShader() : axisLoc(-1), sourceLoc(-1), axis(0,0), source(0)
	{
		name = "bilatBlur";
		type = VertFrag;
	}

	int initialise()
	{
		use();
		axisLoc = glGetUniformLocation(GLID, "axis");
		sourceLoc= glGetUniformLocation(GLID, "source");
		stop();
		return 1;
	}

	void setAxis(glm::ivec2 pAxis)
	{
		axis = pAxis;
	}

	void setSource(int pSource)
	{
		source = pSource;
	}

	void sendUniforms()
	{
		glUniform2iv(axisLoc, 1, &axis[0]);
		glUniform1i(sourceLoc, source);
	}
};