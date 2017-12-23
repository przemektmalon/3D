#pragma once
#include "UIElement.hpp"
#include "Text.hpp"
#include "AssetManager.hpp"
#include "TextShader.hpp"

class UIWindow;

class UILabel : public UIElement
{
public:
	UILabel(UIWindow* pParent);
	~UILabel();

	void init()
	{
		text.init();
		text.setTextOrigin(Text2D::TopLeft);
	}

	void draw()
	{
		text.shader->setProj(parentWindow->getProj());
		text.draw();
	}

	glm::ivec2 getPosition() { return text.getPosition(); }

	void setStyle(Text2D::TextStyle& pStyle)
	{
		text.setStyle(pStyle);
	}

	void setString(std::string&& pStr)
	{
		setString(pStr);
	}

	void setString(std::string& pStr)
	{
		text.setString(pStr);
	}

	void setColour(glm::fvec3 pCol)
	{
		text.setColour(pCol);
	}

	void setCharSize(u8 pSize)
	{
		text.setCharSize(pSize);
	}

	void setFont(Font* pFont)
	{
		text.setFont(pFont);
	}

	void setPosition(glm::fvec2 pPos)
	{
		//pPos += glm::fvec2(parentWindow->borderWidth) + glm::fvec2(0, parentWindow->titleWidth);
		text.setPosition(pPos);
	}

	void setOrigin(glm::ivec2 pOr)
	{
		text.setTextOrigin(pOr);
	}

	void setWindowOrigin(Text2D::Origin pOrigin)
	{
		text.setWindowOrigin(pOrigin);
	}

	void setTextOrigin(glm::ivec2 pOrigin)
	{
		text.setTextOrigin(pOrigin);
	}

	void setTextOrigin(Text2D::Origin pOrigin)
	{
		text.setTextOrigin(pOrigin);
	}

	irect getBoundingBox()
	{
		return text.getBoundingBox();
	}

//private:

	Text2D text;

};

