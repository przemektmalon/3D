#pragma once
#include "Types.h"

struct UIVariable
{
	char* valString;
	union
	{
		double valFloat;
		s64 valInt;
	};
};

struct UIRect
{
	UIRect() {}
	UIRect(s32 pT, s32 pL, s32 pW, s32 pH) : top(pT), left(pL), width(pW), height(pH) {}
	s32 top, left, width, height;
};

struct UISize
{
	s32 width, height;
};