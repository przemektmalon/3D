#include "ui/UIWindowManager.hpp"
#include "Engine.hpp"

void UIWindowManager::checkMouseHovers()
{
	Event ev;
	ev.constructMouse(Mouse::M_NONE, Engine::window.getMousePosition(), 0);
	for (auto w : windows)
	{
		w.second->checkMouseEnter((MouseEvent&)ev);
	}
}
