#pragma once
#include "Window.h"
#include "Shader.h"
//#include "Text.h"
#include <random>

#include <ft2build.h>
#include FT_FREETYPE_H;


class FontStore;
class Mesh;
//class Text;
class Camera;

class Engine
{
public:
	Engine() {}
	~Engine() {}

	static void start();
	static void mainLoop();

	static Shader s;
	static Shader gPassShader;
	static Shader testShader;

	static bool wf;
	static std::mt19937_64 rand;

	//static Text t;

	static Camera defaultOrthoCam;
	//static FontStore fontStore;

	static FT_Library ftLib;
	static s64 startTime;

	static bool movingLight;

	//private:

	static Window window;
	static Mesh uSphere;

};