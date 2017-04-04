#pragma once
#include "Transform.h"
#include "Rect.h"
#include "Engine.h"
#include "Renderer.h"

/*class RectangleShape : public Transformable2D
{
public:
	RectangleShape() 
	{

	}
	~RectangleShape() {}

	void draw()
	{
		glUseProgram(shaderProgramID);
		glBindVertexArray(vaoID);
		glUniform4fv(colourLoc, 1, &colour[0]);
		glBindTextureUnit(0, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	
	void initOGL()
	{
		const auto s = Engine::r->shaderStore.getShader(String32("Shape2DShader"));
		shaderProgram = s;
		GLuint programID = s->getGLID();
		shaderProgramID = programID;
		glUseProgram(programID);

		colourLoc = glGetUniformLocation(programID, "colour");

		glCreateBuffers(1, &vboID);
		glCreateVertexArrays(1, &vaoID);

		glBindVertexArray(vaoID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);

		const auto b = bounds;

		elHeight = 1.f;

		GLfloat tVertices[30] = { 
			b.left, b.top, elHeight, 0, 0,
			b.left, b.bot(), elHeight, 0, 1,
			b.right(), b.top, elHeight, 1, 0,
			b.left, b.bot(), elHeight, 0, 1,
			b.right(), b.bot(), elHeight, 1, 1,
			b.right(), b.top, elHeight, 1, 0 
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 30, tVertices, GL_STATIC_DRAW);

		auto posAttrib = glGetAttribLocation(s->getGLID(), "p");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

		auto texAttrib = glGetAttribLocation(s->getGLID(), "t");
		glEnableVertexAttribArray(texAttrib);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	ShaderProgram* shaderProgram;
	irect bounds;
	GLuint colourLoc;
	glm::fvec4 colour;
	GLuint vboID;
	GLuint vaoID;
	u32 shaderProgramID;

	GLfloat vertices[30];
	float elHeight;
};*/

class RectangleShape
{
public:
	RectangleShape() { proj = glm::ortho<float>(0, 1280, 720, 0); }
	~RectangleShape() {}

	void draw()
	{
		shader->use();
		shader->setUniform(String<64>("projModel"), &proj);
		shader->setUniform(String<64>("colour"), &colour);
		shader->sendUniforms();
		glBindVertexArray(vao);
		glDrawArrays(GL_QUADS, 0, 4);
		glBindVertexArray(0);
	}

	void initOGL()
	{
		shader = Engine::r->shaderStore.getShader(String<32>("Shape2DShader"));
		shader->use();

		glCreateVertexArrays(1, &vao);
		glCreateBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		/*float verts[] = {
			-100,-100,1,0,0,
			-100,100,1,0,0,
			100,100,1,0,0,
			100,-100,1,0,0
		};*/

		//bounds = irect(0, 0, 1280, 180);
		//colour = glm::fvec4(1.f, 0.5, 0.3, 0.2f);

		updateVBO();

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)3);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void updateVBO()
	{
		float verts[] = {
			bounds.left, bounds.top, 1, 0,0,
			bounds.left, bounds.bot(), 1, 0,0,
			bounds.right(), bounds.bot(),1,0,0,
			bounds.right(), bounds.top, 1,0,0
		};

		glNamedBufferData(vbo, sizeof(verts), verts, GL_STATIC_DRAW);
	}

	void setBounds(irect pBounds)
	{
		bounds = pBounds;
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

	ShaderProgram* shader;

private:

	GLuint vao;
	GLuint vbo;
	glm::fvec4 colour;
	irect bounds;
	glm::fmat4 proj;
};