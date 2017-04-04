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

		KC_RETURN = 0x0d,
		KC_SHIFT = 0x10,
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