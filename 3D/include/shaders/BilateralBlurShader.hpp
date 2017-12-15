#pragma once
#include "Shader.hpp"

class BilateralBlurShader : public ShaderProgram
{
public:
	BilateralBlurShader()
	{
		name.overwrite(String32("bilatBlur"));
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

private:
	int axisLoc;
	int sourceLoc;

	glm::ivec2 axis;
	int source;
};