#pragma once
#include "Shader.hpp"

class Shape3DShader : public ShaderProgram
{
public:
	Shape3DShader()
	{
		name.overwrite(String32("Shape3DShader"));
		type = VertFrag;
	}
	~Shape3DShader() {}

	int initialise()
	{
		load(String32("Shape3DShader"), ShaderProgram::VertFrag);
		compile();
		use();
		pvmLoc = glGetUniformLocation(getGLID(), "pvm");
		texLoc = glGetUniformLocation(getGLID(), "tex");
		setTex(0);
		stop();
		return 1;
	}

	void setPVM(glm::fmat4& pPVM)
	{
		pvm = pPVM;
	}

	void setTex(int pTex)
	{
		tex = pTex;
		use();
		glUniform1i(texLoc, tex);
		stop();
	}
	
	void sendUniforms()
	{
		glUniformMatrix4fv(pvmLoc, 1, GL_FALSE, glm::value_ptr(pvm));
	}

private:
	int pvmLoc;
	int texLoc;

	glm::fmat4 pvm;
	int tex;
};

