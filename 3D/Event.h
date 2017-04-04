#pragma once
#include "Keyboard.h"
#include "glm\common.hpp"
#include <queue>

struct Event
{
	enum Type { MouseDown, MouseUp, MouseWheel, KeyDown, KeyUp, WindowResized, WindowMoved };
	Type type;

	Event() {}
	Event(Type pType) : type(pType) {}

	union {
		struct mouse {
			MouseCode code;
			glm::ivec2 position;
			int delta;
		} mouse;
		struct key {
			KeyCode code;
			bool shift;
			bool alt;
			bool sys;
			bool ctrl;
			bool caps;
		} key;
		struct window {
			glm::ivec2 size;
			glm::ivec2 position;
		} window;
	};

	void constructMouse(MouseCode pCode, glm::ivec2 pPos, int pDelta)
	{
		mouse.code = pCode; mouse.position = pPos; mouse.delta = pDelta;
	}

	void constructKey(KeyCode pCode, bool pShift, bool pAlt, bool pSys, bool pCtrl, bool pCaps)
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

private:
	std::queue<Event> events;
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

	KeyCode getCode() { return key.code; }
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