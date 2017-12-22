#pragma once
#include "UIWindowManager.hpp"
#include "Engine.hpp"
#include "RenderConfigWindow.hpp"

UIWindowManager::UIWindowManager()
{
	uiScripts["Render Config"] = renderConfigWindowScripts;
}

void UIWindowManager::loadUIWindows()
{
	addWindow(WindowCreator::createWindow("res/ui/RenderConfigWindow.xml", renderConfigWindowScripts));
}

void UIWindowManager::reloadWindows()
{
	auto w = windows["Render Config"];
	delete w;
	w = WindowCreator::createWindow("res/ui/RenderConfigWindow.xml", renderConfigWindowScripts);
	uiScripts["Render Config"](w);
	windows["Render Config"] = w;
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
