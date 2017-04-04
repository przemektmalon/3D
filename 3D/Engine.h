#pragma once
#include "Window.h"

class Engine
{
public:
	Engine() {}
	~Engine() {}

	static void start();
	static void mainLoop();

private:

	static Window window;

};