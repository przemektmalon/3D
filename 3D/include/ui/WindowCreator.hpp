#pragma once
#include <string>
#include <functional>

class UIWindow;

class WindowCreator
{
public:

	static UIWindow* createWindow(std::string& specPath, std::function<void(UIWindow*)> scripts);
	static UIWindow* createWindow(std::string&& specPath, std::function<void(UIWindow*)> scripts)
	{
		return createWindow(specPath, scripts);
	}

};