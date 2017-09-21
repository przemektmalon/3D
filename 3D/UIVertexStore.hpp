#pragma once
#include "Include.hpp"
#include "Engine.hpp"
#include <vector>
#include "Renderer.hpp"

#define MAX_VERTICES 1024

class UIVertexStore
{
public:
	UIVertexStore() {}
	~UIVertexStore() {}

	u32 pushMeshToBatch(float* pData, u32 dataLength)
	{
		
	}

	void initOGL()
	{
		GLuint programID = Engine::r->shaderStore.getShader(SStr("Shape2DShader"))->getGLID();
		glUseProgram(programID);

		glGenVertexArrays(1, &vaoID);
		glBindVertexArray(vaoID);
		glGenBuffers(1, &vboID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);

		glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES*sizeof(float), NULL, GL_STATIC_DRAW);

		auto posAttrib = glGetAttribLocation(programID, "p");
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(posAttrib);

		auto texAttrib = glGetAttribLocation(programID, "t");
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(texAttrib);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

private:
	
	struct UIRenderMeta
	{
		GLint first;
		GLsizei count;
		GLfloat* data;
		GLint dataSizeInBytes;
	};

	std::map<u32, UIRenderMeta> meta;

	GLuint vboID;
	GLuint vaoID;
};