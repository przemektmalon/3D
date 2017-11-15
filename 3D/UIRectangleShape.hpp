#pragma once
#include "Transform.hpp"
#include "Rect.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "Shape2DShader.hpp"

class RectangleShape
{
public:
	RectangleShape() {}
	~RectangleShape() {}

	void draw()
	{
		shader->use();
		shader->setProjModel(proj);
		shader->setColour(colour);
		shader->sendUniforms();
		glBindVertexArray(vao);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawArrays(GL_QUADS, 0, 4);
		glBindVertexArray(0);
	}

	void initOGL()
	{
		shader = (Shape2DShader*)Engine::r->shaderStore.getShader(String<32>("Shape2DShader"));
		shader->use();

		glCreateVertexArrays(1, &vao);
		glCreateBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		updateVBO();

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void updateVBO()
	{
		float verts[] = {
			bounds.left, bounds.top, 1, 0,0,
			bounds.left, bounds.top + bounds.height, 1, 0,0,
			bounds.right(), bounds.top + bounds.height,1, 0,0,
			bounds.right(), bounds.top, 1, 0,0
		};

		glNamedBufferData(vbo, sizeof(verts), verts, GL_STATIC_DRAW);
	}

	void setBounds(irect pBounds)
	{
		bounds = pBounds;
		updateVBO();
	}

	void setColour(glm::fvec4 pColour)
	{
		colour = pColour;
	}

	void setProj(glm::fmat4 pProj)
	{
		proj = pProj;
	}

	glm::fvec4 getColour()
	{
		return colour;
	}

	irect getBounds()
	{
		return bounds;
	}

	Shape2DShader* shader;

private:

	GLuint vao;
	GLuint vbo;
	glm::fvec4 colour;
	frect bounds;
	glm::fmat4 proj;
};