#pragma once
#include "Engine.h"
//#include "SDL.h"
#include "Include.h"
#include "Shader.h"
#include "Mesh.h"
#include "Time.h"
#include "QPC.h"
#include "SOIL.h"
#include "Camera.h"
#include "Renderer.h"
#include <chrono>
#include "Font.h"
#include "Text.h"
#include <windows.h>
#include <strsafe.h>
#include <functional>
#include "AssetManager.h"

#define MODEL_PATH std::string("res/model/")

#include "World.h"

#include "StringGenerics.h"

#include "UILabel.h"
#include "UIButton.h"

#include "ModelInfo.h"

#include "MeshUtility.h"

#include "Console.h"


FT_Library Engine::ftLib;
Camera Engine::defaultOrthoCam;
HINSTANCE Engine::instance;
std::mt19937_64 Engine::rand;
s64 Engine::startTime;
bool Engine::movingLight;
Engine::EngineState Engine::engineState;
Camera Engine::cam;
Time Engine::dt;
MasterRenderer* Engine::r;
QPC Engine::qpc;
glm::ivec2 Engine::lastM;
UIM Engine::uim;
char Engine::workingDirectory[MAX_PATH];
u32 Engine::workingDirectoryLength;
s32 Engine::selectedID;
UIWindow* Engine::uiw;
bool Engine::windowClicked = false;
glm::ivec2 Engine::clickedPos;
AssetManager Engine::assets;
World* Engine::world;
float Engine::programTime = 0;
Logger Engine::logger;
Window Engine::window;
bool Engine::consoleOpen;
Log Engine::log;
Console Engine::console;
MeshUtility Engine::mu;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>

int main()
{
	Engine::start(GetModuleHandle(NULL));
	return 0;
}

void setRes()
{
	static int curRes = 0;
	--curRes;
	if (curRes < 0)
		curRes = NUM_VALID_RESOLUTIONS - 1;
	Engine::r->setResolution(curRes);
	Engine::window.setResolution(MasterRenderer::getValidResolution(curRes));
	Engine::uiw->updateWindowVBO();
}

void toggleWireFrame()
{
	Engine::r->config.drawWireFrame = !Engine::r->config.drawWireFrame;
}

void toggleTextBounds()
{
	Engine::r->config.drawTextBounds = !Engine::r->config.drawTextBounds;
}

void Engine::start(HINSTANCE pHInstance)
{
	Console::registerConsoleFuncs();

	instance = pHInstance;

	int bytes = GetModuleFileName(NULL, workingDirectory, MAX_PATH);
	if (bytes == 0)
		assert(0);

	DWORD ws = 0;
	bool borderless = true;
	if (borderless)
	{
		ws |= WS_POPUP;
	}
	else
	{
		ws |= (WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
	}

	window.createWindow(instance, "Engine", 0, 0, 1280, 720, ws);
	mainLoop();
}

void Engine::stop()
{
	engineState = Quitting;
}

void Engine::select(glm::ivec2 mPos)
{
	glActiveTexture(GL_TEXTURE0);
	r->fboGBuffer.textureAttachments[3].bind(0);
	glBindTexture(GL_TEXTURE_2D, r->fboGBuffer.textureAttachments[3].getGLID());

	u32* idTex = new u32[r->config.renderResolution.x * r->config.renderResolution.y];

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, idTex);

	u32 source = r->config.renderResolution.x * (r->config.renderResolution.y - 1) + (-1.f * (mPos.y * r->config.renderResolution.x) + (mPos.x));
	auto index = source;

	selectedID = idTex[index];

	//if (selectedID == 0)
	//{
		//selectedID = -1;
	//}

	//Engine::r->tileCullShader->use();
	//Engine::r->tileCullShader->setUniform(String64("selectedID"),&selectedID);
	//Engine::r->tileCullShader->sendUniforms();
	//Engine::r->tileCullShader->stop();

	delete[] idTex;
}

void savePosition()
{
	std::ofstream fstr("pos.dat");
	//fstr.open("res/dat/position.dat");
	if (fstr.is_open())
	{
		fstr.write((char*)&Engine::cam.targetPos, sizeof(glm::fvec2));
		fstr.close();
	}
}

void loadPosition()
{
	std::ifstream fstr("pos.dat");
	//fstr.open("res/dat/position.dat");
	fstr.read((char*)&Engine::cam.targetPos, sizeof(glm::fvec2));
	fstr.close();
}

void moveObjXP()
{
	auto i = Engine::world->getMeshInstance(Engine::selectedID);
	if (!i)
		return;
	//i->sgNode->transform.translate(glm::fvec3(0,0,Engine::dt.getSeconds() * 500));
	i->sgNode->transform.rotate(glm::fvec3(Engine::dt.getSeconds() * 4.5, 0, 0));
	//i->sgNode->transform.setTranslation(glm::fvec3(std::cos(i->sgNode->transform.getRoll()) * 5, std::sin(i->sgNode->transform.getRoll()) * 5, 0));
	i->sgNode->updateAll();
	Engine::world->updateGLBuffers();
}

void moveObjXN()
{
	auto i = Engine::world->getMeshInstance(Engine::selectedID);
	if (!i)
		return;
	//i->sgNode->transform.translate(glm::fvec3(0,0,Engine::dt.getSeconds() * -500));
	i->sgNode->transform.rotate(glm::fvec3(-Engine::dt.getSeconds() * 4.5, 0, 0));
	//i->sgNode->transform.setTranslation(glm::fvec3(std::cos(i->sgNode->transform.getRoll()) * 5, std::sin(i->sgNode->transform.getRoll()) * 5, 0));
	i->sgNode->updateAll();
	Engine::world->updateGLBuffers();
}


void moveObjZP()
{
	auto i = Engine::world->getMeshInstance(Engine::selectedID);
	if (!i)
		return;
	//i->sgNode->transform.translate(glm::fvec3(0,0,Engine::dt.getSeconds() * 500));
	i->sgNode->transform.rotate(glm::fvec3(0, Engine::dt.getSeconds() * 4.5, 0));
	//i->sgNode->transform.setTranslation(glm::fvec3(std::cos(i->sgNode->transform.getRoll()) * 5, std::sin(i->sgNode->transform.getRoll()) * 5, 0));
	i->sgNode->updateAll();
	Engine::world->updateGLBuffers();
}

void moveObjZN()
{
	auto i = Engine::world->getMeshInstance(Engine::selectedID);
	if (!i)
		return;
	//i->sgNode->transform.translate(glm::fvec3(0,0,Engine::dt.getSeconds() * 500));
	i->sgNode->transform.rotate(glm::fvec3(0, Engine::dt.getSeconds() * -4.5, 0));
	//i->sgNode->transform.setTranslation(glm::fvec3(std::cos(i->sgNode->transform.getRoll()) * 5, std::sin(i->sgNode->transform.getRoll()) * 5, 0));
	i->sgNode->updateAll();
	Engine::world->updateGLBuffers();
}

void moveObjYP()
{
	auto i = Engine::world->getMeshInstance(Engine::selectedID);
	if (!i)
		return;
	//i->sgNode->transform.translate(glm::fvec3(0,0,Engine::dt.getSeconds() * 500));
	i->sgNode->transform.rotate(glm::fvec3(0,0,Engine::dt.getSeconds() * 4.5));
	//i->sgNode->transform.setTranslation(glm::fvec3(std::cos(i->sgNode->transform.getRoll()) * 5, std::sin(i->sgNode->transform.getRoll()) * 5, 0));
	i->sgNode->updateAll();
	Engine::world->updateGLBuffers();
}

void moveObjYN()
{
	auto i = Engine::world->getMeshInstance(Engine::selectedID);
	if (!i)
		return;
	//i->sgNode->transform.translate(glm::fvec3(0,0,Engine::dt.getSeconds() * 500));
	i->sgNode->transform.rotate(glm::fvec3(0,0,Engine::dt.getSeconds() * -4.5));
	//i->sgNode->transform.setTranslation(glm::fvec3(std::cos(i->sgNode->transform.getRoll()) * 5, std::sin(i->sgNode->transform.getRoll()) * 5, 0));
	i->sgNode->updateAll();
	Engine::world->updateGLBuffers();
}

void printlog()
{
	Engine::logger.printLog(Engine::log);
}

void mouseDown()
{
	auto mPos = Engine::window.mouse.getWindowPosition(&Engine::window);
	auto uiw = Engine::uiw;
	UIRect wa = uiw->getWindowArea();
	if (mPos.x > wa.left && mPos.x < wa.left + wa.width && mPos.y > wa.top && mPos.y < wa.top + wa.height)
	{
		Engine::windowClicked = true;
		Engine::clickedPos = mPos - glm::ivec2(wa.left,wa.top);
	}
	//uiw->mouseDown();
}

void mouseUp()
{
	Engine::windowClicked = false;
}

void hotLoadShader()
{
	//Engine::r->shaderStore.getShader(String32("gBufferPass"))->reload();
	Engine::r->tileCullShader.reload();
}

void killFocus()
{
	Engine::window.postMessage(WM_KILLFOCUS, 0, 0);
}

void toggleConsole()
{
	Engine::console.toggle();
}

void Engine::mainLoop()
{
	glewExperimental = GL_TRUE;
	glewInit();
	//wglewInit();

	Material::initVertexFormats();
	Material::initDrawModes();
	
	wglSwapIntervalEXT(0);

	uim.mapToKeyDown(VK_ESCAPE, escapePress);
	uim.mapToKeyDown('P', setRes);
	uim.mapToKeyDown('L', screenshot);
	uim.mapToKeyDown('O', toggleWireFrame);
	uim.mapToKeyDown('I', toggleTextBounds);
	uim.mapToKeyDown(VK_OEM_COMMA, loadPosition);
	uim.mapToKeyDown(VK_OEM_PERIOD, savePosition);

	uim.mapToKeyDown('M', hotLoadShader);

	uim.mapToKeyDown(VK_OEM_3, toggleConsole);

	uim.mapToKeyHold(VK_NUMPAD2, moveObjZP);
	uim.mapToKeyHold(VK_NUMPAD8, moveObjZN);
	uim.mapToKeyHold(VK_NUMPAD6, moveObjXP);
	uim.mapToKeyHold(VK_NUMPAD4, moveObjXN);
	uim.mapToKeyHold(VK_NUMPAD9, moveObjYP);
	uim.mapToKeyHold(VK_NUMPAD7, moveObjYN);

	uim.mapToKeyDown('K', printlog);
	uim.mapToKeyDown('Q', killFocus);

	uim.mapToMouseDown(0, mouseDown);
	uim.mapToMouseUp(0, mouseUp);

	r = new MasterRenderer();

	rand.seed(rand.default_seed);

	startTime = std::chrono::system_clock::now().time_since_epoch().count();

	FT_Init_FreeType(&ftLib);

	r->initialiseShaders();

	assets.prepareTexture("res/tex/null.png", "null")->load();

	assets.loader.loadAssets(String128("res/resources.txt"));

	auto me = mu.newObj();
	mu.loadOBJ(String128("res/model/shed.obj"), me);
	mu.setObjName(me, String32("shed"));
	auto binMe = mu.newMesh();
	mu.setMeshName(binMe, String32("shed"));
	mu.binFromObj(me, binMe);
	mu.exportBin(binMe);
	mu.clearStorage();

	/*auto si = mu.objToBin(String<128>("res/model/oo.obj"), String<128>("res/model/oo.bin"), String32("oo"));
	mu.nullAllMeshTextures(si, assets.get2DTex("null"));
	mu.setMeshAlbedoTexture(si, 0, Engine::assets.get2DTex("oo"), 0);
	//mu.setMeshAlbedoTexture(si, 0, Engine::assets.get2DTex("lava"), 1);
	//mu.setMeshAlbedoTexture(si, 0, Engine::assets.get2DTex("grass"), 2);
	//mu.setMeshAlbedoTexture(si, 0, Engine::assets.get2DTex("stone"), 3);
	//mu.setMeshSpecularTexture(si, 0, Engine::assets.get2DTex("stoneS"), 3);
	//mu.setMeshSpecularTexture(si, 0, Engine::assets.get2DTex("terS"), 1);
	//mu.setMeshNormalTexture(si, 0, Engine::assets.get2DTex("stoneN"), 3);
	//mu.setMeshNormalTexture(si, 0, Engine::assets.get2DTex("terN"), 1);
	//mu.setMeshAlphaTexture(si, 0, Engine::assets.get2DTex("alpha"));
	mu.setTriListMaterialID(si, 0, MaterialID::PNUU_T_S_N);
	mu.setMeshName(si, String<32>("oo"));
	mu.scaleAlphaTexCoords(si, 1.f);
	mu.scaleTexCoords(si, 1.f);
	//mu.exportBinV10(si);
	//mu.clearStorage();
	//auto si2 = mu.objToBin(String<128>("res/model/pf.obj"), String<128>("res/model/pf.bin"));
	//mu.setMeshTexture(si2, 0, Engine::assets.get2DTex("pf"), 0);
	//mu.setMeshName(si2, String<32>("pf"));
	//mu.exportBinV10(si);
	//mu.clearStorage();
	//auto si3 = mu.objToBin(String<128>("res/model/sp.obj"), String<128>("res/model/sp.bin"));
	//mu.setMeshTexture(si3, 0, Engine::assets.get2DTex("sp"), 0);
	//mu.setMeshName(si3, String<32>("sp"));
	//mu.exportBinV10(si);
	//mu.clearStorage();

	//mu.addMeshToTriLists(si2, 0, si);
	//mu.addMeshToTriLists(si3, 0, si);

	mu.exportBin(si);
	mu.clearStorage();*/

	

	//auto c = (Mesh*)assets.prepareAsset(Asset::Mesh, "res/model/oo.bin", "paints");

	//a->load();
	//assets.meshManager.pushMeshToBatch(*a);
	//b->load();
	//assets.meshManager.pushMeshToBatch(*b);
	//c->load();
	//assets.meshManager.pushMeshToBatch(*c);

	//d->load();
	//assets.meshManager.pushMeshToBatch(*d);

	assets.meshManager.pushMeshToBatch(*assets.getMesh(String32("shed")));
	assets.meshManager.pushMeshToBatch(*assets.getMesh(String32("square")));

	//ooMesh->load();
	//assets.meshManager.pushMeshToBatch(*ooMesh);
	//pfMesh->load();
	//assets.meshManager.pushMeshToBatch(*pfMesh);
	//spMesh->load();
	//assets.meshManager.pushMeshToBatch(*spMesh);
	
	const GPUMeshManager& mm = Engine::assets.meshManager;

	cam.initialiseProj(float(window.getSizeX()) / float(window.getSizeY()));
	cam.calculateViewRays();
	r->initialiseRenderer(&window, cam);

	defaultOrthoCam.initaliseOrtho(window.getSizeX(), window.getSizeY());

	s64 tot = 0;

	float exposure = 1.f;

	createModelInfoWindow(uiw);

	world = new World();
	world->initialiseGLBuffers();

	auto prevNode = world->getWorldRootNode();

	//auto i2 = world->addMeshInstance(*c, world->getWorldRootNode());
	//i2->sgNode->transform.translate(glm::fvec3(0, -50, 0));
	//i2->sgNode->transform.updateMatrix();

	//auto i3 = world->addMeshInstance(*d, world->getWorldRootNode());
	//i3->sgNode->transform.translate(glm::fvec3(0, 0, 0)).scale(4.f);
	//i3->sgNode->transform.updateMatrix();

	auto e = assets.getMesh(String32("square"));
	auto i4 = world->addMeshInstance(*e, world->getWorldRootNode());
	i4->sgNode->transform.translate(glm::fvec3(0, 0, 0)).scale(50.f);
	i4->sgNode->transform.updateMatrix();

	e = assets.getMesh(String32("shed"));
	auto i5 = world->addMeshInstance(*e, world->getWorldRootNode());
	i5->sgNode->transform.scale(4.f);
	i5->sgNode->transform.updateMatrix();

	/*auto i3 = world->addMeshInstance(*ooMesh, world->getWorldRootNode());
	i3->sgNode->transform.translate(glm::fvec3(0, 20, 0)).scale(10);
	i3->sgNode->transform.updateMatrix();

	auto i4 = world->addMeshInstance(*ooMesh, world->getWorldRootNode());
	i4->sgNode->transform.translate(glm::fvec3(0, 40, 0)).scale(10);
	i4->mesh->triangleListsSorted[0].second[0]->material.albedo[0] = pfTex; ///TODO: This doesnt work beacuse all instances are using the same mesh, which can only have one texture
																			///Maybe disconnect Material, from triangle list. Allow copying of triangle lists (using same data pointer) with different materials
																			///
	i4->sgNode->transform.updateMatrix();

	auto i5 = world->addMeshInstance(*ooMesh, world->getWorldRootNode());
	i5->sgNode->transform.translate(glm::fvec3(0, 60, 0)).scale(10);
	i5->mesh->triangleListsSorted[0].second[0]->material.albedo[0] = spTex;
	i5->sgNode->transform.updateMatrix();

	auto i6 = world->addMeshInstance(*ooMesh, world->getWorldRootNode());
	i6->sgNode->transform.translate(glm::fvec3(0, 80, 0)).scale(10);
	i6->mesh->triangleListsSorted[0].second[0]->material.albedo[0] = Engine::assets.get2DTex("oo");
	i6->sgNode->transform.updateMatrix();*/

	/*auto i4 = world->addMeshInstance(*pfMesh, world->getWorldRootNode());
	i4->sgNode->transform.translate(glm::fvec3(0, 20, 0)).scale(10);
	i4->sgNode->transform.updateMatrix();

	auto i5 = world->addMeshInstance(*spMesh, world->getWorldRootNode());
	i5->sgNode->transform.translate(glm::fvec3(0, 20, 0)).scale(10);
	i5->sgNode->transform.updateMatrix();*/

	world->sg.updateAll();

	world->updateGLBuffers();
	r->world = world;

	console.init();

	while (engineState != Quitting) {
		if (!window.processMessages()) 
		{
			window.keyboard.updateKeyState();

			Event ev;
			while (window.eventQ.pollEvent(ev))
			{
				switch (ev.type)
				{
				case(Event::MouseDown):
				{
					uim.mouseDown(ev.mouse.code);
					uiw->mouseDown((MouseEvent&)ev);
					break;
				}
				case(Event::MouseUp):
				{
					uim.mouseUp(ev.mouse.code);
					uiw->mouseUp((MouseEvent&)ev);
					break;
				}
				case(Event::KeyDown):
				{
					if (console.stateFlags == 0) //Console closed
						uim.keyDown(ev.key.code);
					else						 //Console open
						console.textInput(ev.key.code.code);
					uiw->keyDown((KeyEvent&)ev);
					break;
				}
				case(Event::KeyUp):
				{
					if(console.stateFlags != 0)
						uim.keyUp(ev.key.code);
					uiw->keyUp((KeyEvent&)ev);
					break;
				}
				}
			}

			if(console.stateFlags == 0)
				uim.keyHolds(window.keyboard);

			switch (engineState)
			{
			case(InGame):
			{
				processGameFrame();
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
	qpc.start();

	if (window.mouse.rightDown)
	{
		SetCursorPos(window.getPosX() + (window.getSizeX() / 2), window.getPosY() + (window.getSizeY() / 2));
	}

	if (windowClicked)
	{
		//uiw->drag(window.mouse.getDelta());
		uiw->setWindowPosition(window.mouse.getWindowPosition(&window) - clickedPos);
	}

	const float moveSpeed = 30;

	auto keyboardState = window.keyboard.keyState;

	auto move = glm::fvec3(glm::fvec4(0, 0, moveSpeed * dt.getSeconds(), 1) * cam.matYaw);
	cam.targetPos -= move * float(window.keyboard.isKeyPressed('W'));

	move = glm::cross(glm::fvec3(glm::fvec4(0, 0, moveSpeed * dt.getSeconds(), 1) * cam.matYaw), glm::fvec3(0, 1, 0));
	cam.targetPos += move * float(window.keyboard.isKeyPressed('A'));

	move = glm::fvec3(glm::fvec4(0, 0, moveSpeed * dt.getSeconds(), 1) * cam.matYaw);
	cam.targetPos += move * float(window.keyboard.isKeyPressed('S'));

	move = glm::cross(glm::fvec3(glm::fvec4(0, 0, moveSpeed * dt.getSeconds(), 1) * cam.matYaw), glm::fvec3(0, 1, 0));
	cam.targetPos -= move * float(window.keyboard.isKeyPressed('D'));

	cam.targetPos.y += moveSpeed * dt.getSeconds() * float(window.keyboard.isKeyPressed('R'));

	cam.targetPos.y -= moveSpeed * dt.getSeconds() * float(window.keyboard.isKeyPressed('F'));

	static float exposure = 1.f;

	if (window.keyboard.isKeyPressed('I'))
	{
		exposure += 10 * dt.getSeconds();
		exposure = std::max(0.f, exposure);
		//r->defaultSampler.setTextureAnisotropy(1);
		//r->tileCullShader.use();
		//r->tileCullShader.setExposure(exposure);
	}
	if (window.keyboard.isKeyPressed('U'))
	{
		exposure -= 10 * dt.getSeconds();
		exposure = std::max(0.f, exposure);
		//r->defaultSampler.setTextureAnisotropy(16);
		//r->tileCullShader.use();
		//r->tileCullShader.setExposure(exposure);
	}

	//r->ssaoShader.setIntensity(r->ssaoShader.intensity + (float(keyboardState['B']) * 0.1 * dt.getSeconds()));
	//r->ssaoShader.setIntensity(r->ssaoShader.intensity - (float(keyboardState['V']) * 0.1 * dt.getSeconds()));
	//r->ssaoShader.intensity = std::max(0.f, r->ssaoShader.intensity);

	//r->ssaoShader.setRadius(r->ssaoShader.radius + (float(keyboardState['H']) * 0.1 * dt.getSeconds()));
	//r->ssaoShader.setRadius(r->ssaoShader.radius - (float(keyboardState['G']) * 0.1 * dt.getSeconds()));
	//r->ssaoShader.radius = std::max(0.f, r->ssaoShader.radius);

	cam.update(dt);

	r->render();

	dt.setMicroSeconds(qpc.getElapsedTime());
	
	static float cc;

	cc += dt.getSecondsf();
	programTime += dt.getSecondsf();

	//if (cc > (1.f / 600.f))
	//{
		//log.postMessage(String<32>("Message"));
		//((UILabel*)(uiw->elements[0]))->text.setString(std::string("FPS: " + std::to_string(1.0 / dt.getSeconds()) + '\n' + "Dt: " + std::to_string(dt.getMilliSeconds()) + '\n' +  "Draw Count: " + std::to_string(r->drawCount)).c_str());
		//cc = 0;
	//}
}

void Engine::processMenuFrame()
{

}