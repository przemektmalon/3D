#pragma once
#include <Windows.h>

typedef int KeyCode;

class Key
{
public:
	Key(int pCode) : code(pCode) {}
	Key() : code(KC_NULL) {}
	enum
	{
		/// TODO: Consider if we need an internal key code system. 
		/// Some of the system code defines are not always obvious like VK_MENU == ALT key.
		/// If we want to have an internal system we can expand this list to include all keys including alpha characters

		KC_NULL = 0,

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
	KeyCode code;
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

	bool isKeyPressed(Key keyCode)
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
		M_RIGHT,
		M_MIDDLE,
		M_NONE
	};
	Code code;
};