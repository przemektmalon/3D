#pragma once
#include "UIElement.h"
#include "Text.h"

class UIButton :
	public UIElement
{
public:
	UIButton();
	~UIButton();

	void mouseDown()
	{
		if (text.getColour().x == 1.f)
		{
			text.setColour(glm::fvec3(0.f, 1.f, 0.f));
		}
		else if (text.getColour().y == 1.f)
		{
			text.setColour(glm::fvec3(0.f, 0.f, 1.f));
		}
		else if (text.getColour().z == 1.f)
		{
			text.setColour(glm::fvec3(1.f, 0.f, 0.f));
		}
	}

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

	glm::ivec2 getTopLeft()
	{
		return text.getPosition();
	}

	glm::ivec2 getSize()
	{
		return glm::ivec2(text.getBoundingBox().width, text.getBoundingBox().height);
	}

	//private:

	Text2D text;


};

