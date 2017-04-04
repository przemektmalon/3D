#pragma once
#include <Windows.h>

class KeyCode
{
public:
	KeyCode(unsigned char pKey) : code(Code(pKey)) {}
	enum Code
	{
		KC_BACK = 0x08,
		KC_TAB = 0x09,

		KC_SYS = VK_LWIN,
		KC_RETURN = VK_RETURN,
		KC_SHIFT = VK_SHIFT,
		KC_CTRL = VK_CONTROL,
		KC_ALT = VK_MENU,
		KC_CAPS = VK_CAPITAL,
		KC_ESCAPE = VK_ESCAPE
	};
	Code code;
};

class KeyboardState
{
public:
	KeyboardState() {}
	~KeyboardState() {}

	void updateKeyState()
	{
		GetKeyboardState(PBYTE(&keyState));
	}

	bool isKeyPressed(KeyCode keyCode)
	{
		return (keyState[keyCode.code] >> 1) != 0;
	}

	unsigned char keyState[256];
};

class MouseCode
{
public:
	MouseCode(unsigned char pButton) : code(Code(pButton)) {}
	enum Code
	{
		M_LEFT,
		M_RIGHT
	};
	Code code;
};