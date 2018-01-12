#include "TextBillboard.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "Text.hpp"
#include "AssetManager.hpp"
#include "Texture.hpp"
#include "IL\il.h"
#include "IL\ilu.h"

FBO TextBillboard::fbo;

void TextBillboard::initGL()
{
	text = new Text2D();

	Text2D::TextStyle style(Engine::assets.getFont("consola"), 100);

	text->init();
	text->setStyle(style);
	text->setString(string);
	text->setPosition(glm::fvec2(0.f,-text->getHeight()));
	//text->setTextOrigin(Text2D::Origin::BotLeft);

	auto bounds = text->getBoundingBox();
	glm::ivec2 resolution(bounds.width + 4, bounds.height + 4);

	fbo.create();
	fbo.setResolution(resolution);
	tex = new GLTexture2D();
	tex->createFromStream(GL_RGBA8, bounds.width, bounds.height, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	fbo.checkStatus();
	Billboard::initGL();
}

void TextBillboard::draw()
{
	//tex = Engine::assets.get2DTexGL("light");
	/// TODO: Dont need to redraw this every frame if it doesnt change, just use the same texture

	glViewport(0, 0, tex->getWidth(), tex->getHeight());
	fbo.bind();
	fbo.attachForeignTexture(tex, GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	fbo.clear(GL_COLOR_BUFFER_BIT, glm::fvec4(0.f, 0.f, 0.f, 0.f));
	fbo.bindRead();

	auto bb = text->getBoundingBox();

	//text->shader->setProj(glm::ortho<float>(0, tex->getWidth(), tex->getHeight(), 0, -1, 100));
	text->shader->setProj(glm::ortho<float>(bb.left, bb.right(), bb.top, bb.bot(), -1, 100));
	text->draw();

	glViewport(0, 0, Engine::window.getSizeX(), Engine::window.getSizeY());
	DefaultFBO::bind();

	size.y = (float)tex->getWidth() * 0.04f;
	size.x = (float)tex->getHeight() * 0.04f;

	Billboard::draw();
}


