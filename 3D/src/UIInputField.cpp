#include "ui/UIInputField.hpp"
#include "Engine.hpp"
#include "QPC.hpp"

UIInputField::UIInputField(UIWindow* pParent) : UIElement(InputField, pParent)
{
	colour = glm::fvec4(0.35, 0.35, 0.35, 0.8);
	focusedColour = glm::fvec4(0.4, 0.4, 0.4, 0.9);
}

void UIInputField::onKeyDown(KeyEvent & pKeyEvent)
{
	if (isFocused())
	{
		switch (pKeyEvent.getKey().code)
		{
		case(Key::KC_BACK):
		{
			if (text.getString().length() != 0)
			{
				std::string s = text.getString().c_str();
				s.resize(s.size() - 1);
				text.setString(s);
			}
			break;
		}
		case(Key::KC_DELETE):
		{
			break;
		}
		case(Key::KC_ENTER):
		{
			break;
		}
		case(Key::KC_LEFT):
		{
			break;
		}
		case(Key::KC_RIGHT):
		{
			break;
		}
		default:
		{
			char toAdd = Engine::window.keyboard.convertToASCII(pKeyEvent.getKey());
			if (Key::isPrintable(toAdd))
			{
				text.setString(text.getString() + toAdd);
				if (text.getBoundingBox().right() > rect.getBounds().right())
				{
					std::string s = text.getString().c_str();
					s.resize(s.size() - 1);
					text.setString(s);
				}
			}
		}
		}
		if (text.getString().length() > 0)
		{
			frect newCursor = cursorRect.getBounds();
			newCursor.left = text.getBoundingBox().right() + 1.5;
			cursorRect.setBounds(newCursor);
		}
		else
		{
			frect newCursor = cursorRect.getBounds();
			newCursor.left = rect.getBounds().left + 4;
			cursorRect.setBounds(newCursor);
		}
	}
}

void UIInputField::draw()
{
	rect.setProj(parentWindow->getProj());
	rect.draw();

	text.shader->setProj(parentWindow->getProj());
	text.draw();

	if (isFocused())
	{
		rect.setColour(focusedColour);

		auto mod = Engine::qpc.now() % 1000000;

		if (mod < 500000)
			cursorRect.setColour(glm::fvec4(1.f, 1.f, 1.f, 1.f));
		else
			cursorRect.setColour(glm::fvec4(1.f, 1.f, 1.f, 0.f));
	}
	else
	{
		rect.setColour(colour);
		cursorRect.setColour(glm::fvec4(1.f, 1.f, 1.f, 0.f));
	}

	cursorRect.setProj(parentWindow->getProj());

	cursorRect.draw();
}