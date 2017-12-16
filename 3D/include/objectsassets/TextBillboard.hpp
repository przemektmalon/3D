#pragma once
#include "Billboard.hpp"
#include "Framebuffer.hpp"

class Text2D;

class TextBillboard : public Billboard
{
public:
	TextBillboard(glm::fvec3 pPos, std::string pString) 
	{
		pos = pPos;
		string = pString;
		initGL();
	}
	~TextBillboard() {}

	void initGL();

	void draw();

	std::string string;
	Text2D* text;

	Framebuffer fbo;
};