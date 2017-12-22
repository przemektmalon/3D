#pragma once
#include "UIElement.hpp"
#include "Text.hpp"
#include "UIRectangleShape.hpp"

class UIInputField : public UIElement
{
public:
	UIInputField(UIWindow* pParent);

	void init(glm::fvec2 pos, float width)
	{
		text.init();
		rect.initOGL();
		cursorRect.initOGL();
		focused = false;

		position = pos;
		size = glm::fvec2(width, 24);

		rect.setBounds(irect(pos.x, pos.y, width, 24));
		rect.setColour(glm::fvec4(0.35, 0.35, 0.35, 0.8));

		cursorRect.setBounds(irect(pos.x+4, pos.y + 4, 2, 16));
		cursorRect.setColour(glm::fvec4(1.f, 1.f, 1.f, 0.f));

		text.setTextOrigin(Text2D::Origin::TopLeft);
		text.setPosition(glm::fvec2(pos.x+4, pos.y));
		text.setCharSize(16);
	}

	void onKeyDown(KeyEvent& pKeyEvent);

	void draw();

	Text2D text;
	RectangleShape rect;
	RectangleShape cursorRect;

	glm::fvec4 colour;
	glm::fvec4 focusedColour;

	enum Type { Floating, Integer, String, Vector };

};