#include "Mouse.hpp"
#include "Window.hpp"

glm::ivec2 MouseState::getScreenPosition()
{
	POINT m;
	GetCursorPos(&m);
	return glm::ivec2(m.x, m.y);
}

glm::ivec2 MouseState::getWindowPosition(const Window* pWnd)
{
	POINT m;
	GetCursorPos(&m);
	ScreenToClient(pWnd->windowHandle, &m);
	return glm::ivec2(m.x, m.y);
}