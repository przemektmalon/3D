#pragma once
#include <chrono>
#include <functional>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#include "Engine.hpp"
#include "Include.hpp"
#include "Time.hpp"
#include "QPC.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"
#include "Font.hpp"
#include "Text.hpp"
#include "AssetManager.hpp"
#include "Billboard.hpp"
#include "World.hpp"
#include "Console.hpp"
#include "Tweaks.hpp"

//#include "RenderConfigWindow.hpp"
#include "ProfilingWindow.hpp"
#include "WorldEditWindow.hpp"
#include "WindowCreator.hpp"

// Lambda for passing functions to the profiler
#define CALL(name) []() -> void { Engine::##name##(); }

int main()
{
	Engine::start(GetModuleHandle(NULL));
	return 0;
}

void Engine::start(HINSTANCE pHInstance)
{
	//Console::registerConsoleFuncs();

	instance = pHInstance;

	int bytes = GetModuleFileName(NULL, workingDirectory, MAX_PATH);
	if (bytes == 0)
		assert(0);

	DWORD ws = 0;

	if (BORDERLESS)
	{
		ws |= WS_POPUP;
	}
	else
	{
		ws |= (WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
	}

	SetProcessDPIAware();
	//Sets window to max valid resolution
	int monitorWidth = GetSystemMetrics(SM_CXSCREEN);
	int monitorHeight = GetSystemMetrics(SM_CYSCREEN);
	int windowWidth = 0;
	int windowHeight = 0;
	int resolutionIndex = 5;

	for (int i = 0; i < NUM_VALID_RESOLUTIONS; i++) {
		if (EngineConfig::RenderConfig::validResolutionsRaw[0][i] <= monitorWidth && EngineConfig::RenderConfig::validResolutionsRaw[1][i] <= monitorHeight) {
			windowWidth = EngineConfig::RenderConfig::validResolutionsRaw[0][i];
			windowHeight = EngineConfig::RenderConfig::validResolutionsRaw[1][i];
			resolutionIndex = i;
		}
		if (windowHeight && windowWidth)
			break;
	}

	window.createWindow(instance, "Engine", 0, 0, windowWidth, windowHeight, ws);
	mainLoop(resolutionIndex);
}

void Engine::stop()
{
	engineState = Quitting;
}

void Engine::mainLoop(int resolutionIndex)
{
	glewExperimental = GL_TRUE;
	glewInit();
	FT_Init_FreeType(&ftLib);
	rand.seed(rand.default_seed);
	cam.initialiseProj(float(window.getSizeX()) / float(window.getSizeY()));

	cfg.render.setVSync(false);
	cfg.keyBinds.initialiseFunctionBindingConfig();
	cfg.keyBinds.loadKeyBinds();
	cfg.render.ssao.sampleRadius = 10.f;
	cfg.mouse.sensitivity = glm::fvec2(0.0035, 0.0035);
	cfg.render.minimumLightConstant = 15000;
	cfg.render.drawAABB = false;

	r = new Renderer();
	r->initialiseShaders();

	physicsWorld.create();
	Billboard::initGLVAO();

	assets.modelManager.init();
	assets.loadAssets("res/resources.txt");

	r->initialiseRenderer(&window, cam);

	cfg.render.setResolution(resolutionIndex);
	cfg.render.setFrameScale(1.f);

	world = new World();
	r->world = world;
	r->world->setSkybox("res/skybox/sky/");
	world->initialiseGLBuffers();

	physicsWorld.createGroundPlane();

	for (int i = 0; i < 5; ++i)
	{
		auto instance = world->addModelInstance("pbrsphere");
		instance->setInitialPosition(glm::fvec3(50, 10 + (25 * i), 0));
		instance->makePhysicsObject();
		instance->setScale(rand() % 10 + 5);
		assets.modelManager.pushInstanceAABB(*instance);
		
		if (i % 3 == 0)
			instance->overwriteMaterial(0,0,assets.getMaterial("copper"));
		else if (i % 3 == 1)
			instance->overwriteMaterial(0,0,assets.getMaterial("greasymetal"));
	}

	for (int i = 0; i < 5; ++i)
	{
		auto instance = world->addModelInstance("hollowbox");
		instance->setInitialPosition(glm::fvec3(-50, 20 + (50 * i), 0));
		instance->makePhysicsObject();
		instance->setScale(rand() % 15 + 10);
		assets.modelManager.pushInstanceAABB(*instance);
		
		if (i % 3 == 0)
			instance->overwriteMaterial(0, 0, assets.getMaterial("mahogany"));
		else if (i % 3 == 1)
			instance->overwriteMaterial(0, 0, assets.getMaterial("marble"));
	}

	world->sg.updateAll();
	world->updateGLBuffers();
	
	//console.init();

	Tweaks tweak;
	tweak.setTweaksFile("res/tweaks.txt");
	tweak.setUpdateTime(Time(1));
	tweak.bindVariable(cfg.world.camSpeed, "camSpeed", Tweaks::Floating);
	tweak.bindVariable(Engine::tau, "tau", Tweaks::Floating);
	tweak.bindVariable(Engine::damping, "damping", Tweaks::Floating);
	tweak.bindVariable(Engine::linear, "linear", Tweaks::Floating);
	tweak.bindVariable(Engine::quad, "quad", Tweaks::Floating);

	uiwm.addWindow(createProfilingWindow());
	cfg.world.togglePhysics();

	uiwm.loadUIWindows();

	while (engineState != Quitting) {
		if (!window.processMessages()) 
		{
			window.keyboard.updateKeyState();

			Event ev;
			while (window.eventQ.pollEvent(ev))
			{
				uiwm.passEvent(ev);

				switch (ev.type)
				{
				case(Event::MouseMove):
				{
					if (ev.mouse.code & Mouse::M_RIGHT)
					{
						cam.targetYaw += cfg.mouse.sensitivity.x * ev.mouse.move.x;
						cam.targetPitch += cfg.mouse.sensitivity.y * ev.mouse.move.y;
						SetCursorPos(window.getPosX() + (window.getSizeX() / 2), window.getPosY() + (window.getSizeY() / 2));
					}
					break;
				}
				case(Event::MouseDown):
				{
					uim.mouseDown(ev.mouse.code);

					if (ev.mouse.code & Mouse::M_LEFT)
					{
						glm::fvec3 p = r->activeCam->pos;
						btVector3 camPos(p.x, p.y, p.z);

						btVector3 rayFrom = camPos;
						btVector3 rayTo = physicsWorld.getRayTo(int(ev.mouse.position.x), int(ev.mouse.position.y));

						physicsWorld.pickBody(rayFrom, rayTo);
					}

					break;
				}
				case(Event::MouseUp):
				{
					uim.mouseUp(ev.mouse.code);
					physicsWorld.removePickingConstraint();

					break;
				}
				case(Event::KeyDown):
				{
					//if (console.stateFlags == 0) //Console closed
						uim.keyDown(ev.key.code);
					//else						 //Console open
						//console.textInput(ev.key.code.code);
					break;
				}
				case(Event::KeyUp):
				{
					//if(console.stateFlags != 0)
						uim.keyUp(ev.key.code);
					break;
				}
				}
			}

			uiwm.checkMouseHovers();
			ev.constructMouse(Mouse::M_NONE, Engine::window.getMousePosition(), 0);
			physicsWorld.mouseMoveCallback(ev.mouse.position.x, ev.mouse.position.y);

			//if(console.stateFlags == 0)
				uim.keyHolds(window.keyboard);

			switch (engineState)
			{
			case(InGame):
			{
				tweak.updateTweaks();
				profiler.timeThis(
					CALL(processGameFrame), "frame");
				deltaTime = profiler.getTime("frame");
				programTime += deltaTime.getSecondsf();
				break;
			}
			case(Menu):
			{
				processMenuFrame();
				break;
			}
			}
		}
	}
};

void Engine::processGameFrame()
{
	auto move = glm::fvec3(glm::fvec4(0, 0, cfg.world.camSpeed * deltaTime.getSeconds(), 1) * cam.matYaw);
	cam.targetPos -= move * float(window.keyboard.isKeyPressed('W'));

	move = glm::cross(glm::fvec3(glm::fvec4(0, 0, cfg.world.camSpeed * deltaTime.getSeconds(), 1) * cam.matYaw), glm::fvec3(0, 1, 0));
	cam.targetPos += move * float(window.keyboard.isKeyPressed('A'));

	move = glm::fvec3(glm::fvec4(0, 0, cfg.world.camSpeed * deltaTime.getSeconds(), 1) * cam.matYaw);
	cam.targetPos += move * float(window.keyboard.isKeyPressed('S'));

	move = glm::cross(glm::fvec3(glm::fvec4(0, 0, cfg.world.camSpeed * deltaTime.getSeconds(), 1) * cam.matYaw), glm::fvec3(0, 1, 0));
	cam.targetPos -= move * float(window.keyboard.isKeyPressed('D'));

	cam.targetPos.y += cfg.world.camSpeed * deltaTime.getSeconds() * float(window.keyboard.isKeyPressed('R'));
	cam.targetPos.y -= cfg.world.camSpeed * deltaTime.getSeconds() * float(window.keyboard.isKeyPressed('F'));

	profiler.start("physics");

	if (cfg.world.doPhysics)
	{
		physicsWorld.step(deltaTime);
		physicsWorld.updateModels();
	}

	profiler.end("physics");

	uiwm.updateUIWindows();
	cam.update(deltaTime);

	profiler.glTimeThis(
		CALL(r->render), "render");
}

void Engine::processMenuFrame()
{

}

//********************************* SOME FUNCTIONS / CALLBACKS TO CHANGE CONFIGS *********************************//

void EngineConfig::RenderConfig::setResolution(int validResIndex)
{
	if (validResIndex > NUM_VALID_RESOLUTIONS - 1 || validResIndex < 0)
		return;
	glm::ivec2 oldRes = Engine::cfg.render.getValidResolution(resolutionIndex);
	resolutionIndex = validResIndex;
	resolution = Engine::cfg.render.getValidResolution(resolutionIndex);
	Engine::r->reInitialiseFramebuffers();
	Engine::window.setResolution(Engine::cfg.render.getValidResolution(resolutionIndex));

	Engine::uiwm.updateUIPositionsOnResChange(oldRes, resolution);
}

void EngineConfig::RenderConfig::reloadAllShaders()
{
	Engine::r->shaderStore.reloadAllShaders();
}

void EngineConfig::RenderConfig::screenshot()
{
	Engine::window.screenshot();
}

void EngineConfig::RenderConfig::setRenderMode(int set)
{
	renderMode = set;
	Engine::r->tileCullShader.setRenderMode(renderMode);
}

void printlog()
{
	Engine::engineLog.printLog(Engine::engineLog);
}

void toggleConsole()
{
	//Engine::console.toggle();
}

#define CFG_FUNC(name) []() -> void { Engine::cfg.##name##(); }
void EngineConfig::KeyBindConfig::loadKeyBinds()
{
	/// TODO: Make this more robust
	// As long as input file is correct this will work fine
	// Theres still a few ways we can have bad input, but I've made it much more robust, including comment lines

	// Bad input includes:	spaces after function name
	//						and some other cases probably

	File keyBinds;
	keyBinds.setPath("res/keyBinds.txt");
	keyBinds.open(File::Mode::in);
	
	std::string key;		// Read from file
	std::string funcName;	// Read from file
	
	KeyCode keyCode;		// Generated from read input

	while (!keyBinds.atEOF())
	{
		// Peek at next char to check for empty line (or end of input)
		auto peek = keyBinds.peekChar();
		if (peek == '\n' || peek == '\0')
		{
			keyBinds.pullChar(); // Next line or reach EOF
			continue;
		}

		// Drop spaces lambda
		auto dropSpaces = [&keyBinds]() -> void {
			while (keyBinds.peekChar() == ' '){
				keyBinds.pullChar();}};
		
		dropSpaces(); // Drop leading spaces

		/// TODO: right now '#' must be the first non-whitespace character on a line for it to be a comment
		if (keyBinds.peekChar() == '#')
		{
			std::string commentLine;
			keyBinds.readStr(commentLine, '\n'); // Move read pointer to next line
			continue; // Ignore comment line
		}

		// Read key character
		keyBinds.readStr(key, ' ');

		dropSpaces(); // Drop spaces between key and function

		// Read function name
		keyBinds.readStr(funcName, '\n');

		// Ignore malformed lines. Not sure if this is needed atm
		if (key.size() == 0 || funcName.size() == 0)
			continue;

		// Check if we have the function to bind to
		auto func = functionNames.find(funcName);
		if (func == functionNames.end())
		{
			std::cout << "Function \'" << funcName << "\' not available for binding (check spelling)" << std::endl;
			continue;
		}

		if (key.size() != 1) // Non alpha/printable or special character
		{
			// We need to translate the multi character(byte) code to a system character code the system can understand
			// For example "ESC" is escape, so translate to (char)27

			if (key == "ESC")		// Escape
				keyCode = 27;
			else if (key == "TIL")	// Tilde
				keyCode = 192;
			else if (key == "SCP")	// Space
				keyCode = 32;
			// And we can add more as we need
		}
		else
		{
			// Normal alpha character (if the key binds text file has no errors). It is its own KeyCode (ascii value).
			keyCode = key[0];
		}

		// Reset key and funcname since out File::read function always appends to the provided std::string
		// Consider if this is desired behaviour
		key.clear();
		funcName.clear();
		Engine::uim.mapToKeyDown(keyCode, func->second);
	}

	keyBinds.close();
}


void EngineConfig::KeyBindConfig::initialiseFunctionBindingConfig()
{
	functionNames["escape"] = escapePress;
	functionNames["toggle_textbounds"] = CFG_FUNC(render.toggleDrawTextBounds);
	functionNames["toggle_console"] = CFG_FUNC(render.toggleDrawConsole);
	functionNames["toggle_physics"] = CFG_FUNC(world.togglePhysics);
	functionNames["reload_windows"] = CALL(uiwm.reloadWindows);
}

void EngineConfig::RenderConfig::SSSAOConfig::setFrameScale(float set)
{
	frameScale = set;
}

void EngineConfig::RenderConfig::SSSAOConfig::setProjScale(float set)
{
	projScale = set;
	Engine::r->ssaoShader.setProjScale(projScale);
}

void EngineConfig::RenderConfig::SSSAOConfig::setSampleRadius(float set)
{
	sampleRadius = set;
	Engine::r->ssaoShader.setRadius(sampleRadius);
}

void EngineConfig::RenderConfig::SSSAOConfig::setIntensity(float set)
{
	intensity = set;
	Engine::r->ssaoShader.setIntensity(intensity);
}

void EngineConfig::RenderConfig::SSSAOConfig::setSamples(int set)
{
	samples = set;
	Engine::r->ssaoShader.setSamples(samples);
}

void EngineConfig::RenderConfig::SSSAOConfig::setSpiralTurns(int set)
{
	spiralTurns = set;
	Engine::r->ssaoShader.setSpiralTurns(spiralTurns);
}

void EngineConfig::RenderConfig::CameraConfig::setExposure(float set)
{
	exposure = set;
	Engine::r->tileCullShader.setExposure(exposure);
}

void EngineConfig::RenderConfig::CameraConfig::setFOV(float set)
{
	fov = set;
	Engine::r->activeCam->setFOV(fov);
}

#undef CALL

// Static vars

const s32 EngineConfig::RenderConfig::validResolutionsRaw[2][NUM_VALID_RESOLUTIONS] =
{
	{ 3840 , 1920, 1600, 1536, 1366, 1280, 1024, 960, 848 },
	{ 2160 , 1080, 900,  864,  768 , 720 , 576 , 540, 480 },
};

FT_Library Engine::ftLib;
HINSTANCE Engine::instance;
std::mt19937_64 Engine::rand;
Engine::EngineState Engine::engineState;
Camera Engine::cam;
Time Engine::deltaTime;
Renderer* Engine::r;
QPC Engine::qpc;
UIM Engine::uim;
char Engine::workingDirectory[MAX_PATH];
u32 Engine::workingDirectoryLength;
UIWindowManager Engine::uiwm;
AssetManager Engine::assets;
World* Engine::world;
float Engine::programTime = 0;
Window Engine::window;
bool Engine::consoleOpen;
Log Engine::engineLog;
//Console Engine::console;
PhysicsWorld Engine::physicsWorld;
EngineConfig Engine::cfg;
Profiler Engine::profiler;
float Engine::linear = 0.001;
float Engine::quad = 0.001;
float Engine::tau;
float Engine::damping;