#pragma once
#include "UIWindowManager.hpp"
#include "Engine.hpp"
#include "WindowScripts.hpp"

UIWindowManager::UIWindowManager() {}

void UIWindowManager::loadUIWindows()
{
	addWindow(WindowCreator::createWindow("res/ui/RenderConfigWindow.xml", renderConfigWindowScripts));
	addWindow(WindowCreator::createWindow("res/ui/ProfilingWindow.xml", profilingWindowScripts));
}

void UIWindowManager::reloadWindows()
{
	for (auto& w : windows)
	{
		auto spec = w.second->windowSpecFile;
		auto scripts = w.second->scriptsInitFunc;
		auto name = w.second->getName();

		delete w.second;

		auto hotload = WindowCreator::createWindow(spec, scripts);
		windows[name] = hotload;
	}
}

void UIWindowManager::checkMouseHovers()
{
	Event ev;
	ev.constructMouse(Mouse::M_NONE, Engine::window.getMousePosition(), 0);
	for (auto w : windows)
	{
		w.second->checkMouseEnter((MouseEvent&)ev);
	}
}
