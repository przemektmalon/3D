#pragma once
#include "UIElement.h"
#include "Text.h"
#include "UIRectangleShape.h"

class UIButton :
	public UIElement
{
public:
	UIButton(UIWindow* pParent);
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
		rect.setProj(parentWindow->getProj());
		rect.draw();

		text.shader->setProj(parentWindow->getProj());
		text.shader->setModel(glm::fmat4());
		text.shader->setView(glm::fmat4());
		text.shader->setFontBinding(12);
		text.draw();
	}

	//glm::ivec2 getPosition() { return rect.bounds.topLeft; }
	glm::ivec2 getPosition() { return glm::ivec2(1.f,1.f); }
	glm::ivec2 getTopLeft() { return getPosition(); }
	//glm::ivec2 getSize() { return rect.bounds.size; }
	glm::ivec2 getSize() { return glm::ivec2(1.f,1.f); }

//private:

	Text2D text;
	RectangleShape rect;

};

