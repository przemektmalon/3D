#pragma once
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "MathIncludes.hpp"

#include <queue>

class UIWindow;

struct Event
{
	enum Type { MouseMove, MouseDown, MouseUp, MouseWheel, KeyDown, KeyUp, WindowResized, WindowMoved };
	Type type;

	Event() {}
	Event(Type pType) : type(pType) {}

	union {
		struct mouse_struct {
			MouseCode code;
			glm::ivec2 position;
			glm::ivec2 move;
			int delta;
		};
		mouse_struct mouse;
		struct key_struct {
			Key code;
			bool shift;
			bool alt;
			bool sys;
			bool ctrl;
			bool caps;
		};
		key_struct key;
		struct window_struct {
			glm::ivec2 size;
			glm::ivec2 position;
		};
		window_struct window;
	};

	void constructMouse(MouseCode pCode, glm::ivec2 pPos, int pDelta)
	{
		mouse.code = pCode; mouse.position = pPos; mouse.delta = pDelta;
	}

	void constructKey(Key pCode, bool pShift, bool pAlt, bool pSys, bool pCtrl, bool pCaps)
	{
		key.code = pCode; key.shift = pShift; key.alt = pAlt; key.sys = pSys; key.ctrl = pCtrl; key.caps = pCaps;
	}

	void constructWindow(glm::ivec2 pSize, glm::ivec2 pPos)
	{
		window.size = pSize; window.position = pPos;
	}
};

class EventQ
{
private:
	std::queue<Event> events;

public:
	bool pollEvent(Event& pEvent)
	{
		if (events.size() > 0)
		{
			pEvent = events.front();
			events.pop();
			return true;
		} else
		{
			return false;
		}
	}

	void pushEvent(Event& pEvent)
	{
		events.push(pEvent);
	}
};

class MouseEvent : private Event
{
	friend class UIWindow;
public:
	MouseCode getCode() { return mouse.code; }
	glm::ivec2 getPosition() { return mouse.position; }
	glm::ivec2 getUIWindowPosition(UIWindow* uiw);
	int getDelta() { return mouse.delta; }

private:
	void setPosition(glm::ivec2 pPos) { mouse.position = pPos; }
};

class KeyEvent : private Event
{
public:
	Key getKey() { return key.code; }
	bool getShift() { return key.shift; }
	bool getAlt() { return key.alt; }
	bool getSys() { return key.sys; }
	bool getCtrl() { return key.ctrl; }
	bool getCaps() { return key.caps; }
};

class WindowEvent : private Event
{
public:
	glm::ivec2 getSize() { return window.size; }
	glm::ivec2 getPosition() { return window.position; }
};