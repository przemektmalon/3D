#pragma once
#include "UIUtil.h"
#include "glm\common.hpp"

class UIWindow;

class UIElement
{
public:
	enum ElementType { Label, Button, Slider, SwitchButton };

	UIElement(ElementType elType);
	virtual ~UIElement();

	virtual void draw() = 0;
	virtual void update() {}
	virtual void mouseDown() {}
	const UIVariable& getVar(u32& pVarCount) { return elementVar; }

	virtual glm::ivec2 getTopLeft() { return glm::ivec2(0, 0); }
	virtual glm::ivec2 getSize() { return glm::ivec2(0, 0); }

	void setParentWindow(UIWindow* pWindow)
	{
		parentWindow = pWindow;
	}

	//enum LayoutType { SpanHor, SpanVer, Flow };

protected:
	UIVariable elementVar;
	//u32 varCount;

	UIWindow* parentWindow;

	UISize elementSize;

	const ElementType elementType;
};
