#pragma once
#include "Types.hpp"
#include "MathIncludes.hpp"
#include <string>

struct UIVariable
{
	std::string valueString;
	union
	{
		double valFloat;
		s64 valInt;
	};
};

struct UIRect
{
	UIRect() {}
	UIRect(s32 pL, s32 pT, s32 pW, s32 pH) : top(pT), left(pL), width(pW), height(pH) {}
	s32 left, top, width, height;
};

struct UISize
{
	UISize() {}
	UISize(glm::fvec2 pSize) : width(pSize.x), height(pSize.y) {}
	float width, height;
};