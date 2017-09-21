#pragma once
#include <functional>
#include "Keyboard.hpp"
#include <queue>

void escapePress();


struct ControlArray
{
	std::function<void(void)> keyDownFunctions[256];
	std::function<void(void*)> keyDownFunctionsClass[256];
	std::function<void(void)> keyUpFunctions[256];
	std::function<void(void)> keyHoldFunctions[256];
	std::function<void(void)> mouseDownFunctions[5];
	std::function<void(void)> mouseUpFunctions[5];
};

class UIM
{
public:
	UIM() { ca = &defCA; }
	~UIM() {}

	void mapToKeyDown(KeyCode keyCode, std::function<void(void)> fnc) {
		ca->keyDownFunctions[keyCode.code] = fnc;
	}

	void mapToKeyDown(KeyCode keyCode, std::function<void(void*)> fnc) {
		ca->keyDownFunctionsClass[keyCode.code] = fnc;
	}

	void mapToKeyUp(KeyCode keyCode, std::function<void(void)> fnc)
	{
		ca->keyUpFunctions[keyCode.code] = fnc;
	}

	void mapToKeyHold(KeyCode keyCode, std::function<void(void)> fnc)
	{
		ca->keyHoldFunctions[keyCode.code] = fnc;
	}

	void mapToMouseDown(MouseCode mouseCode, std::function<void(void)> fnc)
	{
		ca->mouseDownFunctions[mouseCode.code] = fnc;
	}

	void mapToMouseUp(MouseCode mouseCode, std::function<void(void)> fnc)
	{
		ca->mouseUpFunctions[mouseCode.code] = fnc;
	}

	void keyDown(KeyCode keyCode)
	{
		auto fnc = ca->keyDownFunctions[keyCode.code];
		if (fnc != nullptr)
		{
			fnc();
		}
		auto fnc2 = ca->keyDownFunctionsClass[keyCode.code];
		if (fnc2 != nullptr)
		{
			char* a = nullptr;
			fnc2(a);
		}
	}

	void keyUp(KeyCode keyCode)
	{
		auto fnc = ca->keyUpFunctions[keyCode.code];
		if (fnc != nullptr)
		{
			fnc();
		}
	}

	void keyHolds(KeyboardState& keyStates)
	{
		//TODO: change to linked list of active holds instead of scanning all keys
		for (int i = 0; i < 256; ++i)
		{
			if (ca->keyHoldFunctions[i] != nullptr && keyStates.isKeyPressed(KeyCode(i)))
			{
				ca->keyHoldFunctions[i]();
			}
		}
	}

	void mouseDown(MouseCode mouseCode)
	{
		auto fnc = ca->mouseDownFunctions[mouseCode.code];
		if (fnc != nullptr)
		{
			fnc();
		}
	}

	void mouseUp(MouseCode mouseCode)
	{
		auto fnc = ca->mouseUpFunctions[mouseCode.code];
		if (fnc != nullptr)
		{
			fnc();
		}
	}

private:

	ControlArray* ca;
	static ControlArray defCA;

};