#pragma once
#include "UIElement.hpp"
#include "UIButton.hpp"
#include <unordered_map>
#include "UIElementContainer.hpp"

class UIMultiTab : public UIElement
{
public:
	UIMultiTab(UIWindow* pParent);

	void addTab(std::string name);

	void addTabElement(std::string tab, UIElement* el)
	{
		tabElements[tab].elements.insert(std::make_pair(el->getName(), el));
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
		back.setBounds(irect(position.x, position.y + 18, dimensions.x, dimensions.y - 18));
	}

	void forceUpdateAll()
	{
		for (auto& tab : tabElements)
			tab.second.update();
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

		tabElements[currentTab].checkMouseEnter(pMouseEvent);
	}

	void draw()
	{
		for (auto& but : tabButtons)
			but.second->draw();

		back.setProj(parentWindow->getProj());
		back.draw();

		if (tabElements.find(currentTab) == tabElements.end())
			return;

		tabElements[currentTab].draw();
	}

	void setCurrentTab(std::string tab)
	{
		currentTab = tab;
	}

private:

	int maxRightTab;
	RectangleShape back;

	std::unordered_map<std::string, UIElementContainer> tabElements;

	std::string currentTab;
	std::unordered_map<std::string, UIButton*> tabButtons;
	//std::unordered_map<std::string, std::vector<UIElement*>> tabElements;
};