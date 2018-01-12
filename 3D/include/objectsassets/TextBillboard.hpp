#pragma once
#include "Billboard.hpp"
#include "Framebuffer.hpp"

class Text2D;
class GLTexture2D;

class TextBillboard : public Billboard
{
public:
	TextBillboard()
	{
		TextBillboard(glm::fvec3(0, 0, 0), "EMPTY");
	}
	TextBillboard(glm::fvec3 pPos, std::string pString) 
	{
		pos = pPos;
		string = pString;
		//initGL();
	}
	~TextBillboard() {}

	void initGL();

	void draw();

	std::string string;
	Text2D* text;

	static void initFBO()
	{
		fbo.create();
	}

	static FBO fbo;
};