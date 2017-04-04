#pragma once
#include "UIUtil.h"
#include "glm\common.hpp"
#include "Event.h"
#include <functional>

class UIWindow;

class UIElement
{
public:
	enum ElementType { Label, Button, Slider, SwitchButton, Console };

	UIElement(ElementType elType, UIWindow* pParent);
	virtual ~UIElement();

	virtual void draw() {}
	virtual void update() {}

	virtual void onKeyDown(KeyEvent& pKeyEvent) 
	{
		if (onKeyDownImpl != nullptr)
		{
			onKeyDownImpl(this, pKeyEvent);
		}
	}
	virtual void onKeyUp(KeyEvent& pKeyEvent) 
	{
		if (onKeyUpImpl != nullptr)
		{
			onKeyUpImpl(this, pKeyEvent);
		}
	}

	virtual void onMouseDown(MouseEvent& pMouseEvent) 
	{
		if (onMouseDownImpl != nullptr)
		{
			onMouseDownImpl(this,pMouseEvent);
		}
	}
	virtual void onMouseUp(MouseEvent& pMouseEvent) 
	{
		if (onMouseUpImpl != nullptr)
		{
			onMouseUpImpl(this, pMouseEvent);
		}
	}

	virtual void onEngineWindowResize(WindowEvent& pWindowEvent) {}
	virtual void onParentWindowResize(WindowEvent& pWindowEvent) {}

	const UIVariable& getVar(u32& pVarCount) { return elementVar; }

	virtual glm::ivec2 getTopLeft() { return glm::ivec2(0, 0); }
	virtual glm::ivec2 getSize() { return glm::ivec2(0, 0); }

	void setParentWindow(UIWindow* pWindow)
	{
		parentWindow = pWindow;
	}

	UIWindow* getParentWindow()
	{
		return parentWindow;
	}

	//enum LayoutType { SpanHor, SpanVer, Flow };

	void setOnKeyboardUp(std::function<void(UIElement*, KeyEvent&)> pOnKeyboardUp)
	{
		onKeyUpImpl = pOnKeyboardUp;
	}

	void setOnKeyboardDown(std::function<void(UIElement*, KeyEvent&)> pOnKeyboardDown)
	{
		onKeyDownImpl = pOnKeyboardDown;
	}

	void setOnMouseUp(std::function<void(UIElement*, MouseEvent&)> pOnMouseUp)
	{
		onMouseUpImpl = pOnMouseUp;
	}

	void setOnMouseDown(std::function<void(UIElement*, MouseEvent&)> pOnMouseDown)
	{
		onMouseDownImpl = pOnMouseDown;
	}

protected:
	UIVariable elementVar;
	//u32 varCount;
	
	UIWindow* parentWindow;
	
	UISize elementSize;

	const ElementType elementType;

	std::function<void(UIElement*, KeyEvent&)> onKeyUpImpl;
	std::function<void(UIElement*, KeyEvent&)> onKeyDownImpl;
	std::function<void(UIElement*, MouseEvent&)> onMouseUpImpl;
	std::function<void(UIElement*, MouseEvent&)> onMouseDownImpl;
};
