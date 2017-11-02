#pragma once
#include "UIUtil.hpp"
#include "glm\common.hpp"
#include "Event.hpp"
#include <functional>
#include "Rect.hpp"

class UIWindow;

class UIElement
{
	friend class UIWindow;
public:
	enum ElementType { Label, Button, Slider, SwitchButton, Console };

	UIElement(ElementType elType, UIWindow* pParent);
	virtual ~UIElement();

	virtual void draw() {}
	virtual void update();

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
			onMouseDownImpl(parentWindow, this, pMouseEvent);	
		}
	}
	virtual void onMouseUp(MouseEvent& pMouseEvent) 
	{
		if (onMouseUpImpl != nullptr)
		{
			onMouseUpImpl(parentWindow, this, pMouseEvent);
		}
	}
	virtual void onMouseEnter(MouseEvent& pMouseEvent)
	{
		if (onMouseEnterImpl != nullptr)
		{
			onMouseEnterImpl(parentWindow, this, pMouseEvent);
		}
	}

	virtual void onMouseLeave(MouseEvent& pMouseEvent)
	{
		if (onMouseLeaveImpl != nullptr)
		{
			onMouseLeaveImpl(parentWindow, this, pMouseEvent);
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

	frect getBounds()
	{
		return frect(position.x, position.y, dimensions.width, dimensions.height);
	}

	void setUpdate(std::function<void(UIWindow*, UIElement*)> pUpdate)
	{
		updateImpl = pUpdate;
	}

	void setOnKeyboardUp(std::function<void(UIWindow*, UIElement*, KeyEvent&)> pOnKeyboardUp)
	{
		onKeyUpImpl = pOnKeyboardUp;
	}

	void setOnKeyboardDown(std::function<void(UIWindow*, UIElement*, KeyEvent&)> pOnKeyboardDown)
	{
		onKeyDownImpl = pOnKeyboardDown;
	}

	void setOnMouseUp(std::function<void(UIWindow*, UIElement*, MouseEvent&)> pOnMouseUp)
	{
		onMouseUpImpl = pOnMouseUp;
	}

	void setOnMouseDown(std::function<void(UIWindow*, UIElement*, MouseEvent&)> pOnMouseDown)
	{
		onMouseDownImpl = pOnMouseDown;
	}

	void setOnMouseEnter(std::function<void(UIWindow*, UIElement*, MouseEvent&)> pOnMouseEnter)
	{
		onMouseEnterImpl = pOnMouseEnter;
	}

	void setOnMouseLeave(std::function<void(UIWindow*, UIElement*, MouseEvent&)> pOnMouseLeave)
	{
		onMouseLeaveImpl = pOnMouseLeave;
	}

	bool isClicked()
	{
		return clicked;
	}

	bool isHovered()
	{
		return hovered;
	}

	bool isOffClick()
	{
		return offClick;
	}

	void setHovered(bool state)
	{
		hovered = state;
	}

	void setClicked(bool state)
	{
		clicked = state;
	}

	void setOffClick(bool state)
	{
		offClick = state;
	}

protected:

	UIVariable elementVar;
	//u32 varCount;
	
	UIWindow* parentWindow;
	
	glm::fvec2 position;
	UISize dimensions;

	bool clicked;
	bool hovered;
	bool offClick;

	const ElementType elementType;

	float updateInterval;
	float updateClock;
	std::function<void(UIWindow*, UIElement*)> initImpl;
	std::function<void(UIWindow*, UIElement*)> updateImpl;
	std::function<void(UIWindow*, UIElement*, KeyEvent&)> onKeyUpImpl;
	std::function<void(UIWindow*, UIElement*, KeyEvent&)> onKeyDownImpl;
	std::function<void(UIWindow*, UIElement*, MouseEvent&)> onMouseUpImpl;
	std::function<void(UIWindow*, UIElement*, MouseEvent&)> onMouseDownImpl;
	std::function<void(UIWindow*, UIElement*, MouseEvent&)> onMouseEnterImpl;
	std::function<void(UIWindow*, UIElement*, MouseEvent&)> onMouseLeaveImpl;
};
