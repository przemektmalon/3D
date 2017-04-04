#pragma once
#include "UIUtil.h"

class UIWindow;

class UIElement
{
public:
	enum ElementType { Label, Button, Slider, SwitchButton };

	UIElement(ElementType elType);
	virtual ~UIElement();

	virtual void draw() = 0;
	const UIVariable& getVar(u32& pVarCount) { return elementVar; }

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
