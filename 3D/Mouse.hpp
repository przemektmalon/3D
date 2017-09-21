#pragma once
#include "Include.hpp"

class Window;

class MouseState
{
public:
	MouseState() {}
	~MouseState() {}

	glm::ivec2 getScreenPosition();
	glm::ivec2 getWindowPosition(const Window* pWnd);

	void setDelta(glm::ivec2 pD) { delta = pD; }
	glm::ivec2 getDelta() { return delta; }

	bool leftDown;
	bool rightDown;
	bool middleDown;
	glm::ivec2 delta;
};