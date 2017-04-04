#pragma once

#include "Window.h"
#include "Shader.h"
//#include "Text.h"
#include <random>
#include "UserInputManager.h"

#include "UIWindow.h"

#include <ft2build.h>
#include FT_FREETYPE_H;

class FontStore;
class Mesh;
//class Text;
class Camera;
class Time;
class MasterRenderer;
class QPC;
class ResourceManager;

class Engine
{
public:
	Engine() {}
	~Engine() {}

	static void start(HINSTANCE pHInstance);
	static void mainLoop();
	static void processFrame();
	static void stop();
	static void select(glm::ivec2 mPos);

	static u64 h1;
	static u64 h2;
	static u64 h3;

	static GLTexture2DMip t1;
	static GLTexture2DMip t2;
	static GLTexture2DMip t3;
	static GLTexture2DMip t4;

	static Shader s;
	static Shader gPassShader;
	static Shader testShader;

	static std::mt19937_64 rand;

	//static Text t;

	static Camera defaultOrthoCam;
	//static FontStore fontStore;

	static FT_Library ftLib;
	static s64 startTime;

	static bool movingLight;
	static Camera cam;
	static Time dt;
	static MasterRenderer* r;
	static QPC qpc;
	static glm::ivec2 lastM;
	static UIM uim;

	static u32 selectedID;

	static UIWindow* uiw;
	static bool windowClicked;
	static glm::ivec2 clickedPos;

	static ResourceManager resMan;

	//private:

	enum EngineState { InGame, Menu, Quitting };

	static EngineState engineState;
	static Window window;

	static HINSTANCE instance;
	static char workingDirectory[MAX_PATH];
	static u32 workingDirectoryLength;
};