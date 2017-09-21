#pragma once
#include "UIUtil.hpp"
#include "glm\common.hpp"
#include "Event.hpp"
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
			onKeyDownImpl(parentWindow, this, pKeyEvent);
		}
	}
	virtual void onKeyUp(KeyEvent& pKeyEvent) 
	{
		if (onKeyUpImpl != nullptr)
		{
			onKeyUpImpl(parentWindow, this, pKeyEvent);
		}
	}
	virtual void onMouseDown(MouseEvent& pMouseEvent) 
	{
		if (onMouseDownImpl != nullptr)
		{
			onMouseDownImpl(parentWindow, this,pMouseEvent);
		}
	}
	virtual void onMouseUp(MouseEvent& pMouseEvent) 
	{
		if (onMouseUpImpl != nullptr)
		{
			onMouseUpImpl(parentWindow, this, pMouseEvent);
		}
	}
	virtual void onHover(MouseEvent& pMouseEvent)
	{
		if (onHoverImpl != nullptr)
		{
			onHoverImpl(parentWindow, this, pMouseEvent);
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

	void setOnKeyboardUp(std::function<bool(UIWindow*, UIElement*, KeyEvent&)> pOnKeyboardUp)
	{
		onKeyUpImpl = pOnKeyboardUp;
	}

	void setOnKeyboardDown(std::function<bool(UIWindow*, UIElement*, KeyEvent&)> pOnKeyboardDown)
	{
		onKeyDownImpl = pOnKeyboardDown;
	}

	void setOnMouseUp(std::function<bool(UIWindow*, UIElement*, MouseEvent&)> pOnMouseUp)
	{
		onMouseUpImpl = pOnMouseUp;
	}

	void setOnMouseDown(std::function<bool(UIWindow*, UIElement*, MouseEvent&)> pOnMouseDown)
	{
		onMouseDownImpl = pOnMouseDown;
	}

	void setOnHover(std::function<bool(UIWindow*, UIElement*, MouseEvent&)> pOnHover)
	{
		onHoverImpl = pOnHover;
	}

protected:
	UIVariable elementVar;
	//u32 varCount;
	
	UIWindow* parentWindow;
	
	UISize elementSize;

	const ElementType elementType;

	std::function<bool(UIWindow*, UIElement*, KeyEvent&)> onKeyUpImpl;
	std::function<bool(UIWindow*, UIElement*, KeyEvent&)> onKeyDownImpl;
	std::function<bool(UIWindow*, UIElement*, MouseEvent&) > onMouseUpImpl;
	std::function<bool(UIWindow*, UIElement*, MouseEvent&)> onMouseDownImpl;
	std::function<bool(UIWindow*, UIElement*, MouseEvent&)> onHoverImpl;
};
