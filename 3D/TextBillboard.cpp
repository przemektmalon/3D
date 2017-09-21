#include "TextBillboard.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "Text3D.hpp"

void TextBillboard::initGL()
{
	text = new Text3D();
	text->init();
	text->setString(String32("BILLBOARD BILLBOARD BILLBOARD\nBILLBOARD BILLBOARD BILLBOARD"));
	text->setPosition(glm::fvec3(2.f, 2.f, 2.f));
}

void TextBillboard::draw()
{
	text->draw();
}
