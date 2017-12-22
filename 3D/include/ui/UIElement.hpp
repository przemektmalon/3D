#pragma once
#include "glm\common.hpp"
#include "Event.hpp"
#include <functional>
#include "Rect.hpp"

class UIWindow;

class UIElement
{
	friend class UIWindow;
public:
	enum ElementType { Label, Button, Slider, SwitchButton, InputField, MultiTab, Console };

	UIElement(ElementType elType, UIWindow* pParent);
	virtual ~UIElement();

	void setName(std::string pName)
	{
		name = pName;
	}

	std::string& getName()
	{
		return name;
	}

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

	void setPosition(glm::fvec2 pPos) { position = pPos; }
	void setSize(glm::fvec2 pDim) { size = pDim; }

	virtual void onEngineWindowResize(WindowEvent& pWindowEvent) {}
	virtual void onParentWindowResize(WindowEvent& pWindowEvent) {}

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
		return frect(position.x, position.y, size.x, size.y);
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

	bool isFocused()
	{
		return focused;
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

	void setFocused(bool state)
	{
		focused = state;
	}

protected:

	std::string name;
	
	UIWindow* parentWindow;
	
	glm::fvec2 position;
	glm::fvec2 size;

	bool clicked;
	bool hovered;
	bool offClick;
	bool focused;

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
