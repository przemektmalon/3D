#pragma once
#include "Window.h"
#include "Shader.h"

class Engine
{
public:
	Engine() {}
	~Engine() {}

	static void start();
	static void mainLoop();

	static Shader s;

	//private:

	static Window window;

};