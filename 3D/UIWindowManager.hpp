#pragma once
#include "UIWindow.hpp"
#include "Event.hpp"

class UIWindowManager
{
public:
	UIWindowManager() {}
	~UIWindowManager() {}

	void drawUIWindows()
	{
		for (auto w : windows)
			w.second->draw();
	}

	void addWindow(UIWindow* uiw, glm::ivec2 position = glm::ivec2(-1,-1))
	{
		if (position != glm::ivec2(-1, -1))
			uiw->setWindowPosition(position);

		windows.insert(std::make_pair(uiw->getName(), uiw));
	}

	void passEvent(Event& ev)
	{
		for (auto w : windows)
		{
			if (!w.second->isOpen())
			{
				switch (ev.type)
				{
				case(Event::MouseDown):
				{
					w.second->mouseDown((MouseEvent&)ev);
					break;
				}
				case(Event::MouseUp):
				{
					w.second->mouseUp((MouseEvent&)ev);
					break;
				}
				case(Event::KeyDown):
				{
					w.second->keyDown((KeyEvent&)ev);
					break;
				}
				case(Event::KeyUp):
				{
					w.second->keyUp((KeyEvent&)ev);
					break;
				}
				}
			}
		}
	}

	void checkMouseHovers();

	void updateUIWindows()
	{
		for (auto w : windows)
			w.second->update();
	}

	void updateUIPositionsOnResChange(glm::ivec2 oldRes, glm::ivec2 newRes)
	{
		for (auto w : windows)
		{
			auto& win = *w.second;
			glm::fvec2 ratio((float)win.getWindowArea().left / (float)oldRes.x, (float)win.getWindowArea().top / (float)oldRes.y);
			win.setWindowPosition(glm::ivec2(ratio.x * float(newRes.x), ratio.y * float(newRes.y)));
		}
	}

private:

	std::unordered_map<std::string,UIWindow*> windows;

};