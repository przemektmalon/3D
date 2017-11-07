#pragma once
#include "UIWindow.hpp"


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

	void addWindow(UIWindow* uiw)
	{
		windows.insert(std::make_pair(uiw->getName(), uiw));
	}

private:

	std::unordered_map<std::string,UIWindow*> windows;

};