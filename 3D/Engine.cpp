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
//FontStore Engine::fontStore;
Camera Engine::defaultOrthoCam;
//Text Engine::t;
HINSTANCE Engine::instance;
//Shader Engine::s;
//Shader Engine::testShader;
//Shader Engine::gPassShader;
std::mt19937_64 Engine::rand;
s64 Engine::startTime;
//Mesh Engine::uSphere;
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include "cmder.h"

int main()
{
	Engine::start(GetModuleHandle(NULL));
	return 0;
}

/*int WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd)
{
	Engine::start(hInstance);

	//HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	return 0;
}*/

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

void toggleWF()
{
	Engine::r->rC.wireFrame = !Engine::r->rC.wireFrame;
}


void Engine::start(HINSTANCE pHInstance)
{
	//TESTING CONSOLE
	Console::registerConsoleFuncs();

	//console.submitCommand(String512("FUNCNAME (1,5,6) (2.3 2.4) 5 \"STRING\" 10000.0002 9999"));
	//console.submitCommand(String512("FUNC 5"));
	//console.submitCommand(String512("FOO 1 5.0"));
	//TESTING CONSOLE

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

	u32* idTex = new u32[r->rC.renderResolution.x * r->rC.renderResolution.y];

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, idTex);

	u32 source = r->rC.renderResolution.x * (r->rC.renderResolution.y - 1) + (-1.f * (mPos.y * r->rC.renderResolution.x) + (mPos.x));
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
	uim.mapToKeyDown('O', toggleWF);
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
	//assets.initMeshBatch();

	auto nullTex = assets.prepareTexture("res/tex/null.png", "null"); nullTex->load();

	auto t1p = assets.prepareMipTexture("res/tex/g.jpg", "g"); t1p->load();
	auto t2p = assets.prepareMipTexture("res/tex/gN.jpg", "gN"); t2p->load();
	auto t3p = assets.prepareMipTexture("res/tex/gS.jpg", "gS"); t3p->load();
	auto t4p = assets.prepareMipTexture("res/tex/gB.jpg", "gB"); t4p->load();

	auto ooTex = assets.prepareMipTexture("res/tex/oo.jpg", "oo"); ooTex->load();
	auto pfTex = assets.prepareMipTexture("res/tex/pf.jpg", "pf"); pfTex->load();
	//auto spTex = (GLTexture2D*)assets.prepareAsset(Asset::Texture2DMip, "res/tex/sp.jpg", "sp"); spTex->load();
	auto stoneTex = assets.prepareMipTexture("res/tex/stone.png", "stone"); stoneTex->load();
	auto terTex = assets.prepareMipTexture("res/tex/ter.png", "ter"); terTex->load();

	auto stoneNTex = assets.prepareMipTexture("res/tex/stoneN.png", "stoneN"); stoneNTex->load();
	auto terNTex = assets.prepareMipTexture("res/tex/terN.png", "terN"); terNTex->load();

	auto alphaTex = (GLTexture2D*)assets.prepareMipTexture("res/tex/alpha.png", "alpha"); alphaTex->load();

	auto stone2Tex = assets.prepareMipTexture("res/tex/stone2.jpg", "stone2"); stone2Tex->load();
	auto grassTex = assets.prepareMipTexture("res/tex/grass.png", "grass"); grassTex->load();
	auto lavaTex = assets.prepareMipTexture("res/tex/lava.png", "lava"); lavaTex->load();
	auto dirtTex = assets.prepareMipTexture("res/tex/dirt.jpg", "dirt"); dirtTex->load();

	auto a = assets.prepareMesh("res/model/box.bin", "box");
	auto b = assets.prepareMesh("res/model/sceneNEW.bin", "ter");
	
	assets.prepareFont("res/fonts/clear-sans/ClearSans-Regular.ttf", "clearsans")->load();
	assets.prepareFont("res/fonts/clear-sans/ClearSans-Bold.ttf", "clearsansb")->load();

	///TODO: Automate mesh utulity through world_object definition files
	///These should contain paths to triangle lists (.OBJ), texture names for each tri-list, world_object mesh name
	///AND binary OUTPUT path

	///Example definition file

	/*
	
	[0]
		"res/model/OUTPUT.bin"
		"OUTPUT_MESH_NAME"
	[1]
		"res/model/SCENE.obj"
		"g"
	[2]
		"res/model/BOX.obj"
		"some_other_texture"
	[3]
		...
		...

	*/

	///END

	//OBJ TO BIN

	String<128> objPath, binPath;
	String<32> meshName, texName;

	MeshUtility mu;
	auto si = mu.objToBin(String<128>("res/model/square.obj"), String<128>("res/model/square.bin"));
	mu.nullAllMeshTextures(si, nullTex);
	mu.setMeshAlbedoTexture(si, 0, Engine::assets.get2DTex("dirt"), 0);
	mu.setMeshAlbedoTexture(si, 0, Engine::assets.get2DTex("lava"), 1);
	mu.setMeshAlbedoTexture(si, 0, Engine::assets.get2DTex("grass"), 2);
	mu.setMeshAlbedoTexture(si, 0, Engine::assets.get2DTex("stone"), 3);
	//mu.setMeshSpecularTexture(si, 0, Engine::assets.get2DTex("stoneS"), 3);
	//mu.setMeshSpecularTexture(si, 0, Engine::assets.get2DTex("terS"), 1);
	//mu.setMeshNormalTexture(si, 0, Engine::assets.get2DTex("stoneN"), 3);
	//mu.setMeshNormalTexture(si, 0, Engine::assets.get2DTex("terN"), 1);
	mu.setMeshAlphaTexture(si, 0, Engine::assets.get2DTex("alpha"));
	mu.setTriListMaterialID(si, 0, MaterialID::PNUU_TT_SS_NN);
	mu.setMeshName(si, String<32>("square"));
	mu.scaleAlphaTexCoords(si, 1.f / 50.f);
	mu.scaleTexCoords(si, 50.f);
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

	mu.exportBinV11(si);

	mu.clearStorage();

	auto c = (Mesh*)assets.prepareAsset(Asset::Mesh, "res/model/oo.bin", "paints");
	auto d = (Mesh*)assets.prepareAsset(Asset::Mesh, "res/model/square.bin", "square");


	//a->load();
	//assets.meshManager.pushMeshToBatch(*a);
	//b->load();
	//assets.meshManager.pushMeshToBatch(*b);
	c->load();
	assets.meshManager.pushMeshToBatch(*c);

	d->loadBinV11();
	d->sortTriangleLists();
	assets.meshManager.pushMeshToBatch(*d);

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

	auto i2 = world->addMeshInstance(*c, world->getWorldRootNode());
	i2->sgNode->transform.translate(glm::fvec3(0, -50, 0));
	i2->sgNode->transform.updateMatrix();

	auto i3 = world->addMeshInstance(*d, world->getWorldRootNode());
	i3->sgNode->transform.translate(glm::fvec3(0, 10, 0)).scale(50.f);
	i3->sgNode->transform.updateMatrix();

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
	
	//static float cc;

	//cc += dt.getSecondsf();
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