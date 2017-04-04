#pragma once
#include "UIRectangleShape.h"
#include "Text.h"
#include "UIElement.h"

class UIConsole
{
public:
	UIConsole() {}
	~UIConsole() {}

	void initOGL()
	{
		proj = glm::ortho<float>(0, Engine::window.getSizeX(), 0, consoleHeight);

		rect.initOGL();

		rect.setBounds(irect(0, 0, Engine::window.getSizeX(), consoleHeight));
		rect.setColour(glm::fvec4(1.f, 0.5, 0.3, 0.5f));
		rect.updateVBO();

		for (int i = 0; i < 10; ++i)
		{
			/*Text2D* t;
			t = new Text2D();
			t->init();
			t->setFont(Engine::assets.getFont("clearsans"));
			t->setWindowSize(glm::ivec2(1280, 200));
			t->setWindowOrigin(Text2D::BotLeft);
			t->setCharSize(12);
			t->setString("LINE 1 AAAAAAAAAAAAAAAAAAAAAAAA");
			t->setTextOrigin(glm::ivec2(0, 0));
			t->setPosition(glm::fvec2(10, 10 + (i * 15)));
			t->setColour(glm::fvec3(0.95f, 0.95f, 0.95f));
			lines.push_back(t);*/
		}
	}

	void draw()
	{
		glViewport(0, 0, Engine::window.getSizeX(), Engine::window.getSizeY());
		glm::fmat4 p = glm::ortho<float>(0, Engine::window.getSizeX(), 0, Engine::window.getSizeY());
		rect.shader->setUniform(String<64>("projModel"), &p);
		rect.draw();
		glViewport(0, Engine::window.getSizeY() - consoleHeight, Engine::window.getSizeX(), consoleHeight);
		for (auto itr = lines.begin(); itr != lines.end(); ++itr)
		{
			(*itr)->shader->setProj(proj);
			(*itr)->shader->setView(glm::fmat4());
			(*itr)->draw();
		}
	}

	void onEngineWindowResize(glm::ivec2 pNewSize)
	{
		proj = glm::ortho<float>(0, Engine::window.getSizeX(), 0, consoleHeight);
	}

private:

	int consoleHeight = 200;
	glm::fmat4 proj;
	std::list<Text2D*> lines;
	RectangleShape rect;
};