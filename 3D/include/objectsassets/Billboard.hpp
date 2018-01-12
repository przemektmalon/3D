#pragma once
#include "MathIncludes.hpp"
#include "GLIncludes.hpp"
#include "VertexArray.hpp"
#include "BufferObjects.hpp"

class Shape3DShader;
class GLTexture2D;

static const float squareVerts[] = {
	-0.5f, -0.5f, 0.0f, 0.f, 1.f,
	0.5f, -0.5f, 0.0f, 0.f, 0.f,
	0.5f, 0.5f, 0.0f, 1.f, 0.f,
	-0.5f, 0.5f, 0.0f, 1.f, 1.f
};

class Billboard
{
public:
	Billboard() : constantSize(true) {}
	Billboard(glm::fvec3 pPos, glm::fvec2 pSize, GLTexture2D* pTex) : size(pSize), pos(pPos), tex(pTex), constantSize(true) {}
	~Billboard() {}

	static void initGLVAO();

	void initGL();

	void draw();

	glm::fvec2 size;
	glm::fvec3 pos;
	GLTexture2D* tex;

	//GLuint vbo, vao;

	VAO vao;
	VBO vbo;

	bool constantSize;

private:

	static Shape3DShader* shader;
};