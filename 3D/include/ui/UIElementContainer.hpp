#pragma once
#include "UIElement.hpp"
#include <unordered_map>

class UIElementContainer
{
public:

	std::unordered_map<std::string, UIElement*> elements;

	UIElement* getElement(std::string pName)
	{
		for (auto& el : elements)
		{
			if (el.second->getName() == pName)
				return el.second;
		}
		return nullptr;
	}

	virtual void update()
	{
		for (auto& el : elements)
			el.second->update();
	}

	virtual void draw()
	{
		for (auto& el : elements)
			el.second->draw();
	}

	virtual bool mouseDown(MouseEvent& pMouseEvent)
	{
		for (auto& el : elements)
			el.second->setFocused(false);
		
		bool canDrag = true;

		for (auto& el : elements)
		{
			auto rect = el.second->getBounds();

			auto winMP = pMouseEvent.getUIWindowPosition(el.second->getParentWindow());

			if (!rect.contains(winMP))
				continue;

			el.second->setFocused(true);
			el.second->setClicked(true);
			el.second->onMouseDown(pMouseEvent);
			canDrag = false;
		}
		return canDrag;
	}

	virtual void mouseUp(MouseEvent& pMouseEvent)
	{
		for (auto& el : elements)
		{
			if (el.second->isClicked())
			{
				el.second->setClicked(false);
				el.second->onMouseUp(pMouseEvent);
			}
		}
	}

	virtual void keyDown(KeyEvent & pKeyEvent)
	{
		for (auto& el : elements)
		{
			el.second->onKeyDown(pKeyEvent);
		}
	}

	virtual void keyUp(KeyEvent & pKeyEvent)
	{
		for (auto& el : elements)
		{
			el.second->onKeyUp(pKeyEvent);
		}
	}

	virtual void checkMouseEnter(MouseEvent& pMouseEvent)
	{
		auto mp = pMouseEvent.getPosition();

		for (auto& el : elements)
		{
			auto rect = el.second->getBounds();

			auto winMP = pMouseEvent.getUIWindowPosition(el.second->getParentWindow());

			if (!rect.contains(winMP))
			{
				if (el.second->isHovered())
				{
					el.second->setHovered(false);
					el.second->onMouseLeave(pMouseEvent);
					if (el.second->isClicked())
						el.second->setOffClick(true);
				}
				continue;
			}

			if (!el.second->isHovered())
			{
				el.second->setHovered(true);
				el.second->onMouseEnter(pMouseEvent);
				if (el.second->isClicked())
					el.second->setOffClick(false);
			}
		}
	}
};