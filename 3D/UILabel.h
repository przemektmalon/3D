#pragma once
#include "UIElement.h"
#include "Text.h"

class UILabel :
	public UIElement
{
public:
	UILabel();
	~UILabel();

	void draw()
	{
		text.shader->setProj(parentWindow->getProj());
		text.shader->setModel(glm::fmat4());
		text.shader->setView(glm::fmat4());
		text.shader->setFontBinding(12);
		text.draw();
	}

	glm::ivec2 getPosition() { //return text.getPosition(); }
	}

	//private:

	Text2D text;

};

