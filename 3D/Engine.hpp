#pragma once

#include "Window.hpp"
#include "Shader.hpp"
#include <random>
#include "UserInputManager.hpp"
#include "Log.hpp"
#include "UIWindow.hpp"
#include <unordered_map>
#include <ft2build.h>
#include FT_FREETYPE_H;

#include "PhysicsWorld.hpp"

#include "UIWindowManager.hpp"

#include "Profiler.hpp"

class FontStore;
class Camera;
class Time;
class Renderer;
class QPC;
class AssetManager;
class World;
class Console;

#define BORDERLESS true
#define NUM_VALID_RESOLUTIONS 9

class EngineConfig
{
public:

	struct RenderConfig {

		struct SSSAOConfig {
			float frameScale;
			void setFrameScale(float set);

			float projScale;
			void setProjScale(float set);

			float sampleRadius;
			void setSampleRadius(float set);

			float intensity;
			void setIntensity(float set);

			int samples;
			void setSamples(int set);

			int spiralTurns;
			void setSpiralTurns(int set);
		} ssao;

		struct CameraConfig {
			float exposure;
			void setExposure(float set);

			float fov;
			void setFOV(float set);
		} camera;

		static const s32 validResolutionsRaw[2][NUM_VALID_RESOLUTIONS];
		static const glm::ivec2 getValidResolution(int pIndex) {
			return glm::ivec2(validResolutionsRaw[0][pIndex], validResolutionsRaw[1][pIndex]);
		}

		glm::ivec2 resolution;
		s32 resolutionIndex;
		float frameScale;
		void setResolution(int validResIndex);
		void cycleRes() { setResolution(resolutionIndex == 0 ? NUM_VALID_RESOLUTIONS - 1 : resolutionIndex - 1); }
		int getResolutionIndex() { return resolutionIndex; }
		void setFrameScale(float set) { frameScale = set; }
		glm::ivec2 getResolution() { return resolution; }

		bool vSync;
		void setVSync(bool set) { vSync = set; wglSwapIntervalEXT(vSync); }
		void toggleVSync() { setVSync(!vSync); }

		bool drawWireframe;
		void setDrawWireFrame(bool set) { drawWireframe = set; }
		void toggleDrawWireframe() { drawWireframe = !drawWireframe; }

		bool drawTextBounds;
		void setDrawTextBounds(bool set) { drawTextBounds = set; }
		void toggleDrawTextBounds() { drawTextBounds = !drawTextBounds; }

		void reloadAllShaders();

		void screenshot();

		bool drawConsole;
		void setDrawConsole(bool set) { drawConsole = set; }
		void toggleDrawConsole() { drawConsole = !drawConsole; }

		//Higher Constant == LOWER  MINUMUM LIGHT
		//Lower  Constant == HIGHER MINIMUM LIGHT
		float minimumLightConstant;
		void setMinimumLightConstant(float set) { minimumLightConstant = set; }

		int renderMode;
		void setRenderMode(int set);

		//FOG
		//AA level
		//Tonemap settings (exposure + curve parameters)
		//Motion blur
		//DoF
		//Vignetting
		//draw AABB

	} render;

	struct WorldConfig {

		float camSpeed;

	} world;

	struct KeyBindConfig {
		std::unordered_map<std::string, std::function<void(void)>> functionNames;
		
		/*struct KeyBindContext
		{
			KeyBindContext(std::string pName, std::)
				std::string name; //eg. menu, in-game etc
		};*/

		void initialiseFunctionBindingConfig();
		//Load file with key binds and load file with mouse bind and set both
		void loadKeyBinds();
	
	} keyBinds;

	struct MouseConfig {
		glm::fvec2 sensitivity;
	} mouse;
};

class Engine
{
public:
	Engine() {}
	~Engine() {}

	static void start(HINSTANCE pHInstance);
	static void mainLoop(int resolutionIndex);
	static void processGameFrame();
	static void processMenuFrame();
	static void stop();

	static World* world;

	static float linear, quad;
	static int doPhysics;

	static float programTime;

	static std::mt19937_64 rand;

	static FT_Library ftLib;
	static s64 startTime;

	static Console console;
	static bool movingLight;
	static Camera cam;
	static Time dt;
	static Renderer* r;
	static QPC qpc;
	static UIM uim;

	static UIWindowManager uiwm;
	static UIWindow* uiw;
	static Log engineLog;
	
	static AssetManager assets;

	static bool consoleOpen;

	static Physics p;
	static PhysicsWorld physics;

	static float tau, damping;

	static EngineConfig cfg;

	static Profiler profiler;

	static u64 physicsTime;
	static u64 frameTime;

//private:

	enum EngineState { InGame, Menu, Quitting };

	static EngineState engineState;
	static Window window;

	static HINSTANCE instance;
	static char workingDirectory[MAX_PATH];
	static u32 workingDirectoryLength;
};