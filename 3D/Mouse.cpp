#include "Mouse.hpp"
#include "Window.hpp"

glm::ivec2 Mouse::getScreenPosition()
{
	POINT m;
	GetCursorPos(&m);
	return glm::ivec2(m.x, m.y);
}

glm::ivec2 Mouse::getWindowPosition(const Window* pWnd)
{
	POINT m;
	GetCursorPos(&m);
	ScreenToClient(pWnd->windowHandle, &m);
	return glm::ivec2(m.x, m.y);
}