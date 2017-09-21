#pragma once
#include "Include.hpp"
#include "Shape3DShader.hpp"
#include "Texture.hpp"

class Text3D;

class TextBillboard
{
public:
	TextBillboard() {}
	~TextBillboard() {}

	void initGL();

	void draw();

	glm::fvec3 pos;

	Text3D* text;
};