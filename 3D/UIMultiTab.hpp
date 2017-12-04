#pragma once
#include "UIElement.hpp"
#include "UIButton.hpp"
#include <unordered_map>

class UIMultiTab : public UIElement
{
public:
	UIMultiTab(UIWindow* pParent);

	void addTab(std::string name);

	void addTabElement(std::string tab, UIElement* el)
	{

	}

	void setPosition(glm::fvec2 pos)
	{
		position = pos;
		maxRightTab = pos.x;
		back.setBounds(irect(position.x, position.y + 18, dimensions.x, dimensions.y));
	}

	void setDimensions(glm::fvec2 dim)
	{
		dimensions = dim;
		back.setBounds(irect(position.x, position.y + 18, dimensions.x, dimensions.y));
	}

	void update()
	{
		for (auto& but : tabButtons)
			but.second->update();
	}

	void mouseDown(MouseEvent& pMouseEvent)
	{
		for (auto itr = tabButtons.begin(); itr != tabButtons.end(); ++itr)
		{
			auto rect = (itr)->second->getBounds();

			auto winMP = pMouseEvent.getUIWindowPosition((itr)->second->getParentWindow());

			if (!rect.contains(winMP))
				continue;

			(itr)->second->setFocused(true);
			(itr)->second->setClicked(true);
			(itr)->second->onMouseDown(pMouseEvent);
		}

		if (tabElements.find(currentTab) == tabElements.end())
			return;

		for (auto itr = tabElements[currentTab].begin(); itr != tabElements[currentTab].end(); ++itr)
			(*itr)->setFocused(false);

		auto mp = pMouseEvent.getPosition();

		for (auto itr = tabElements[currentTab].begin(); itr != tabElements[currentTab].end(); ++itr)
		{
			auto rect = (*itr)->getBounds();

			auto winMP = pMouseEvent.getUIWindowPosition((*itr)->getParentWindow());

			if (!rect.contains(winMP))
				continue;

			(*itr)->setFocused(true);
			(*itr)->setClicked(true);
			(*itr)->onMouseDown(pMouseEvent);
		}
	}

	void mouseUp(MouseEvent& pMouseEvent)
	{
		for (auto itr = tabButtons.begin(); itr != tabButtons.end(); ++itr)
		{
			if ((itr)->second->isClicked())
			{
				(itr)->second->setClicked(false);
				(itr)->second->onMouseUp(pMouseEvent);
			}
		}

		if (tabElements.find(currentTab) == tabElements.end())
			return;

		for (auto itr = tabElements[currentTab].begin(); itr != tabElements[currentTab].end(); ++itr)
		{
			if ((*itr)->isClicked())
			{
				(*itr)->setClicked(false);
				(*itr)->onMouseUp(pMouseEvent);
			}
		}
	}

	void keyDown(KeyEvent & pKeyEvent)
	{
		if (tabElements.find(currentTab) == tabElements.end())
			return;

		for (auto itr = tabElements[currentTab].begin(); itr != tabElements[currentTab].end(); ++itr)
		{
			(*itr)->onKeyDown(pKeyEvent);
		}
	}

	void keyUp(KeyEvent & pKeyEvent)
	{
		if (tabElements.find(currentTab) == tabElements.end())
			return;

		for (auto itr = tabElements[currentTab].begin(); itr != tabElements[currentTab].end(); ++itr)
		{
			(*itr)->onKeyUp(pKeyEvent);
		}
	}

	void checkMouseEnter(MouseEvent& pMouseEvent)
	{
		auto mp = pMouseEvent.getPosition();

		for (auto itr = tabButtons.begin(); itr != tabButtons.end(); ++itr)
		{
			auto rect = (itr)->second->getBounds();

			auto winMP = pMouseEvent.getUIWindowPosition((itr)->second->getParentWindow());

			if (!rect.contains(winMP))
			{
				if ((itr)->second->isHovered())
				{
					(itr)->second->setHovered(false);
					(itr)->second->onMouseLeave(pMouseEvent);
					if ((itr)->second->isClicked())
						(itr)->second->setOffClick(true);
				}
				continue;
			}

			if (!(itr)->second->isHovered())
			{
				(itr)->second->setHovered(true);
				(itr)->second->onMouseEnter(pMouseEvent);
				if ((itr)->second->isClicked())
					(itr)->second->setOffClick(false);
			}
		}

		if (tabElements.find(currentTab) == tabElements.end())
			return;

		for (auto itr = tabElements[currentTab].begin(); itr != tabElements[currentTab].end(); ++itr)
		{
			auto rect = (*itr)->getBounds();

			auto winMP = pMouseEvent.getUIWindowPosition((*itr)->getParentWindow());

			if (!rect.contains(winMP))
			{
				if ((*itr)->isHovered())
				{
					(*itr)->setHovered(false);
					(*itr)->onMouseLeave(pMouseEvent);
					if ((*itr)->isClicked())
						(*itr)->setOffClick(true);
				}
				continue;
			}

			if (!(*itr)->isHovered())
			{
				(*itr)->setHovered(true);
				(*itr)->onMouseEnter(pMouseEvent);
				if ((*itr)->isClicked())
					(*itr)->setOffClick(false);
			}
		}
	}

	void draw()
	{
		for (auto& but : tabButtons)
			but.second->draw();

		back.setProj(parentWindow->getProj());
		back.draw();
	}

private:

	int maxRightTab;
	RectangleShape back;

	std::string currentTab;
	std::unordered_map<std::string, UIButton*> tabButtons;
	std::unordered_map<std::string, std::vector<UIElement*>> tabElements;
};