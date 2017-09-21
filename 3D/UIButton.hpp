#pragma once
#include "UIElement.hpp"
#include "Text.hpp"
#include "UIRectangleShape.hpp"

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

	void setString(StringGeneric& str)
	{
		text.setString(str);
		updateBounds();
	}

	void setPosition(glm::fvec2 pos)
	{
		position = pos;
		updateBounds();
	}

	void setSize(glm::fvec2 pSize)
	{
		size = pSize;
		updateBounds();
	}

	void updateBounds()
	{
		auto b = text.getBoundingBox();
		border.x = (size.x - b.width) * 0.5;
		border.y = (size.y - b.height) * 0.5;
		text.setPosition(position + border);
		rect.setBounds(frect(position.x, position.y, (border.x*2.f) + b.width, (border.y * 2.f) + b.height));
	}

	void setRectColour(glm::fvec4 pCol)
	{
		rect.setColour(pCol);
	}

	void setTextColour(glm::fvec3 pCol)
	{
		text.setColour(pCol);
	}

	Text2D& getText()
	{
		return text;
	}

	frect getBounds()
	{
		return frect(position.x, position.y, size.x, size.y);
	}

	//glm::ivec2 getPosition() { return rect.bounds.topLeft; }
	glm::ivec2 getPosition() { return glm::ivec2(1.f,1.f); }
	glm::ivec2 getTopLeft() { return getPosition(); }
	//glm::ivec2 getSize() { return rect.bounds.size; }
	glm::ivec2 getSize() { return glm::ivec2(1.f,1.f); }



private:

	Text2D text;
	RectangleShape rect;
	glm::fvec2 size;
	glm::fvec2 border;
	glm::fvec2 position;
};

