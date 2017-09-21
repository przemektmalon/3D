#pragma once
#include "Include.hpp"
#include "Shape3DShader.hpp"
#include "Texture.hpp"

static const float squareVerts[] = {
	-0.5f, -0.5f, 0.0f, 0.f, 1.f,
	0.5f, -0.5f, 0.0f, 0.f, 0.f,
	0.5f, 0.5f, 0.0f, 1.f, 0.f,
	-0.5f, 0.5f, 0.0f, 1.f, 1.f
};

class Billboard
{
public:
	Billboard() {}
	Billboard(glm::fvec3 pPos, glm::fvec2 pSize, GLTexture2D* pTex) : size(pSize), pos(pPos), tex(pTex) {}
	~Billboard() {}

	static void initGLVAO();

	void initGL();

	void draw();

	glm::fvec2 size;
	glm::fvec3 pos;
	GLTexture2D* tex;

	GLuint vbo, vao;

private:

	static Shape3DShader* shader;
};