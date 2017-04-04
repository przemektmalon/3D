#pragma once
#include "Shader.h"

class TextShader : public Shader
{
public:
	TextShader() { initialise(); }

	void initialise()
	{
		load("res/shader/text", "res/shader/text");
		use();
		fontLoc = glGetUniformLocation(program, "font");
		projLoc = glGetUniformLocation(program, "proj");
		viewLoc = glGetUniformLocation(program, "view");
		modelLoc = glGetUniformLocation(program, "model");
		setFontBinding(12);
		stop();
	}

	void setFontBinding(int pBinding)
	{
		use();
		glUniform1i(fontLoc, pBinding);
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

private:
	int fontLoc;
	int projLoc;
	int viewLoc;
	int modelLoc;
};