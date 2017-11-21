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

		KC_BACK = VK_BACK,
		KC_DELETE = VK_DELETE,
		KC_ENTER = VK_RETURN,
		KC_TAB = VK_TAB,
		KC_LEFT = VK_LEFT,
		KC_RIGHT = VK_RIGHT,

		KC_SYS = VK_LWIN,
		KC_RETURN = VK_RETURN,
		KC_SHIFT = VK_SHIFT,
		KC_CTRL = VK_CONTROL,
		KC_ALT = VK_MENU,
		KC_CAPS = VK_CAPITAL,
		KC_ESCAPE = VK_ESCAPE
	};
	KeyCode code;

	static bool isNum(char c)
	{
		return c > 47 && c < 58;
	}

	static bool isAlpha(char c)
	{
		return (c > 64 && c < 91) || (c > 96 && c < 123);
	}

	static bool isPrintable(char c)
	{
		return (c > 31 && c < 127);
	}
};

class Keyboard
{
public:
	Keyboard() {}
	~Keyboard() {}

	void updateKeyState()
	{
		GetKeyboardState(PBYTE(&keyState));
	}

	bool isKeyPressed(Key keyCode)
	{
		return (keyState[keyCode.code] >> 1) != 0;
	}

	char convertToASCII(Key key);
	
	unsigned char keyState[256];
};