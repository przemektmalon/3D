#pragma once
#include "MathIncludes.hpp"

typedef int MouseCode;

class Window;

class Mouse
{
public:
	Mouse() {}
	~Mouse() {}

	glm::ivec2 getScreenPosition();
	glm::ivec2 getWindowPosition(const Window* pWnd);

	void setDelta(glm::ivec2 pD) { delta = pD; }
	glm::ivec2 getDelta() { return delta; }

	enum Code
	{
		M_NONE = 0,
		M_LEFT = 1,
		M_RIGHT = 2,
		M_MIDDLE = 4
	};

	MouseCode state;
	glm::ivec2 delta;
};