#pragma once
#include "Window.h"
#include "Shader.h"
#include <random>

class Engine
{
public:
	Engine() {}
	~Engine() {}

	static void start();
	static void mainLoop();

	static Shader s;
	static Shader gPassShader;

	static bool wf;
	static std::mt19937_64 rand;

	//private:

	static Window window;

};