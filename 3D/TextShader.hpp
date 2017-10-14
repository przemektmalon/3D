#pragma once
#include "Shader.hpp"

class TextShader : public ShaderProgram
{
public:
	TextShader() { 
		initialise(); 
	}
	
	int initialise()
	{
		load("text", VertFrag);
		compile();
		use();
		fontLoc = glGetUniformLocation(GLID, "font");
		projLoc = glGetUniformLocation(GLID, "proj");
		viewLoc = glGetUniformLocation(GLID, "view");
		modelLoc = glGetUniformLocation(GLID, "model");
		colourLoc = glGetUniformLocation(GLID, "colour");
		setFontBinding(12);
		stop();
		return 1;
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

	void setColour(glm::fvec3& colour)
	{
		use();
		glUniform3fv(colourLoc, 1, &colour[0]);
	}

private:
	int fontLoc;
	int projLoc;
	int viewLoc;
	int modelLoc;
	int colourLoc;
};