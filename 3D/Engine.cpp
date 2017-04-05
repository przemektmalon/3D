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

int WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd)
{
	Engine::start(hInstance);

	//HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

	return 0;
}

static int count = 0;

void setRes()
{
	--count;
	if (count < 0)
		count = NUM_VALID_RESOLUTIONS - 1;
	Engine::r->setResolution(count);
	Engine::window.setResolution(MasterRenderer::getValidResolution(count));
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

	if (selectedID == 0)
	{
		selectedID = -1;
	}

	//Engine::r->tileCullShader->use();
	//Engine::r->tileCullShader->setUniform(String64("selectedID"),&selectedID);
	//Engine::r->tileCullShader->sendUniforms();
	//Engine::r->tileCullShader->stop();

	delete[] idTex;
}

void specifyScreenVertexAttributes(GLuint shaderProgram)
{
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
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
	if (Engine::selectedID == -1) return;
	auto i = Engine::world->getMeshInstance(Engine::selectedID);
	//i->sgNode->transform.translate(glm::fvec3(0,0,Engine::dt.getSeconds() * 500));
	i->sgNode->transform.rotate(glm::fvec3(Engine::dt.getSeconds() * 4.5, 0, 0));
	//i->sgNode->transform.setTranslation(glm::fvec3(std::cos(i->sgNode->transform.getRoll()) * 5, std::sin(i->sgNode->transform.getRoll()) * 5, 0));
	i->sgNode->updateAll();
	Engine::world->updateGLBuffers();
}

void moveObjXN()
{
	if (Engine::selectedID == -1) return;
	auto i = Engine::world->getMeshInstance(Engine::selectedID);
	//i->sgNode->transform.translate(glm::fvec3(0,0,Engine::dt.getSeconds() * -500));
	i->sgNode->transform.rotate(glm::fvec3(-Engine::dt.getSeconds() * 4.5, 0, 0));
	//i->sgNode->transform.setTranslation(glm::fvec3(std::cos(i->sgNode->transform.getRoll()) * 5, std::sin(i->sgNode->transform.getRoll()) * 5, 0));
	i->sgNode->updateAll();
	Engine::world->updateGLBuffers();
}


void moveObjZP()
{
	if (Engine::selectedID == -1) return;
	auto i = Engine::world->getMeshInstance(Engine::selectedID);
	//i->sgNode->transform.translate(glm::fvec3(0,0,Engine::dt.getSeconds() * 500));
	i->sgNode->transform.rotate(glm::fvec3(0, Engine::dt.getSeconds() * 4.5, 0));
	//i->sgNode->transform.setTranslation(glm::fvec3(std::cos(i->sgNode->transform.getRoll()) * 5, std::sin(i->sgNode->transform.getRoll()) * 5, 0));
	i->sgNode->updateAll();
	Engine::world->updateGLBuffers();
}

void moveObjZN()
{
	if (Engine::selectedID == -1) return;
	auto i = Engine::world->getMeshInstance(Engine::selectedID);
	//i->sgNode->transform.translate(glm::fvec3(0,0,Engine::dt.getSeconds() * 500));
	i->sgNode->transform.rotate(glm::fvec3(0, Engine::dt.getSeconds() * -4.5, 0));
	//i->sgNode->transform.setTranslation(glm::fvec3(std::cos(i->sgNode->transform.getRoll()) * 5, std::sin(i->sgNode->transform.getRoll()) * 5, 0));
	i->sgNode->updateAll();
	Engine::world->updateGLBuffers();
}

void moveObjYP()
{
	if (Engine::selectedID == -1) return;
	auto i = Engine::world->getMeshInstance(Engine::selectedID);
	//i->sgNode->transform.translate(glm::fvec3(0,0,Engine::dt.getSeconds() * 500));
	i->sgNode->transform.rotate(glm::fvec3(0,0,Engine::dt.getSeconds() * 4.5));
	//i->sgNode->transform.setTranslation(glm::fvec3(std::cos(i->sgNode->transform.getRoll()) * 5, std::sin(i->sgNode->transform.getRoll()) * 5, 0));
	i->sgNode->updateAll();
	Engine::world->updateGLBuffers();
}

void moveObjYN()
{
	if (Engine::selectedID == -1) return;
	auto i = Engine::world->getMeshInstance(Engine::selectedID);
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

	uim.mapToMouseDown(0,mouseDown);
	uim.mapToMouseUp(0,mouseUp);

	r = new MasterRenderer();

	rand.seed(rand.default_seed);

	startTime = std::chrono::system_clock::now().time_since_epoch().count();


	//HeapString ab("A");

	//ab.append("BCDhelloworld");
	//ab.append("EFGhelloworldHIJK").removeCases("helloworld");
	//ab.insertAt(4, "4").insertWhere("BCD\0","OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\0");

	FT_Init_FreeType(&ftLib);
	//fontStore.initialise();

	//Font font;
	//font.load(size, "gor.ttf", fc);

	//t.init();
	//t.setFont(&font);
	//t.setCharSize(size);
	//t.setString("Hello!");

	//s.load("res/shader/Standard", "res/shader/Standard");
	//gPassShader.load("res/shader/gBufferPass", "res/shader/gBufferPass");
	//testShader.load("res/shader/test", "res/shader/test");
	//glUseProgram(s());
	//Mesh m;
	//m.loadBinary("MONKEY.bin");
	////m.load("MONKEY2.obj");
	////m.saveBinary("MONKEY.bin");

	//Mesh m2;
	//m2.load("GROUND.obj");
	//m2.transform.setRoll(PI);
	//m2.transform.setScale(glm::fvec3(10.f));
	//m2.transform.setTranslation(glm::fvec3(0, -80, 0));

	//Mesh buddha(MODEL_PATH + "buddha.bin");
	//Mesh dragon(MODEL_PATH + "DRG.bin");
	//Mesh ballpyr(MODEL_PATH + "ballpyr.bin");
	//Mesh table;
	//table.load(MODEL_PATH + "table.obj");
	//ballpyr.load(MODEL_PATH + "ballpyr.obj");
	//ballpyr.saveBinary(MODEL_PATH + "ballpyr.bin");
	//Mesh floor;
	//floor.load(MODEL_PATH + "Plane.obj");
	//Mesh surf;
	//surf.load(MODEL_PATH + "surface.obj");

	//Mesh sp;
	//sp.load(MODEL_PATH + "sp.obj");

	//Mesh oo;
	//oo.load(MODEL_PATH + "oo.obj");

	//Mesh pf;
	//pf.load(MODEL_PATH + "pf.obj");

	//Mesh ter;
	//ter.load(MODEL_PATH + "newTer.obj");

	//sp.tex = new GLTexture2D();
	//sp.tex->createFromFile("res/tex/sp.jpg");

	//uSphere.load(MODEL_PATH + "UnitSphere.obj");

	r->initialiseShaders();
	assets.initMeshBatch();

	//t1.createFromFile(SSTR("res/tex/g.jpg"));
	//t2.createFromFile(SSTR("res/tex/gN.jpg"));
	//t3.createFromFile(SSTR("res/tex/gD.jpg"));
	//t4.createFromFile(SSTR("res/tex/gB.jpg"));

	auto t1p = (GLTexture2D*)assets.prepareAsset(Asset::Texture2DMip, "res/tex/g.jpg", "g"); t1p->load();
	auto t2p = (GLTexture2D*)assets.prepareAsset(Asset::Texture2DMip, "res/tex/gN.jpg", "gN"); t2p->load();
	auto t3p = (GLTexture2D*)assets.prepareAsset(Asset::Texture2DMip, "res/tex/gS.jpg", "gS"); t3p->load();
	auto t4p = (GLTexture2D*)assets.prepareAsset(Asset::Texture2DMip, "res/tex/gB.jpg", "gB"); t4p->load();

	auto ooTex = (GLTexture2D*)assets.prepareAsset(Asset::Texture2DMip, "res/tex/oo.jpg", "oo"); ooTex->load();
	//auto pfTex = (GLTexture2D*)assets.prepareAsset(Asset::Texture2DMip, "res/tex/pf.jpg", "pf"); pfTex->load();
	//auto spTex = (GLTexture2D*)assets.prepareAsset(Asset::Texture2DMip, "res/tex/sp.jpg", "sp"); spTex->load();

	auto ooNTex = (GLTexture2D*)assets.prepareAsset(Asset::Texture2DMip, "res/tex/ooN.jpg", "ooN"); ooNTex->load();
	//auto pfNTex = (GLTexture2D*)assets.prepareAsset(Asset::Texture2DMip, "res/tex/pfN.jpg", "pfN"); pfNTex->load();
	//auto spNTex = (GLTexture2D*)assets.prepareAsset(Asset::Texture2DMip, "res/tex/spN.jpg", "spN"); spNTex->load();

	//char b3[] = "res/model/oo.bin";
	//char b3n[] = "oo";

	auto a = (Mesh*)assets.prepareAsset(Asset::Mesh, "res/model/box.bin", "box");
	auto b = (Mesh*)assets.prepareAsset(Asset::Mesh, "res/model/sceneNEW.bin", "ter");
	//auto c = resMan.registerMesh(b3, b3n);

	auto ooMesh = (Mesh*)assets.prepareAsset(Asset::Mesh, "res/model/oo.bin", "oo");
	//auto pfMesh = (Mesh*)assets.prepareAsset(Asset::Mesh, "res/model/pf.bin", "pf");
	//auto spMesh = (Mesh*)assets.prepareAsset(Asset::Mesh, "res/model/sp.bin", "sp");

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
	auto si = mu.objToBin(String<128>("res/model/oo.obj"), String<128>("res/model/oo.bin"));
	mu.setMeshTexture(si, 0, Engine::assets.get2DTex("oo"), 0);
	mu.setMeshName(si, String<32>("oo"));
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

	mu.exportBinV10(si);

	mu.clearStorage();

	a->load();
	assets.meshManager.pushMeshToBatch(*a);
	b->load();
	assets.meshManager.pushMeshToBatch(*b);

	ooMesh->load();
	assets.meshManager.pushMeshToBatch(*ooMesh);
	//pfMesh->load();
	//assets.meshManager.pushMeshToBatch(*pfMesh);
	//spMesh->load();
	//assets.meshManager.pushMeshToBatch(*spMesh);
	
	const GPUMeshManager& mm = Engine::assets.meshManager;

	/*const u32 maxObjects = 65536;

	GLCMD* cmds = new GLCMD[maxObjects];

	auto eggMesh = assets.getMesh("egg");
	auto bid = eggMesh->renderMeta.batchID;
	auto bi = eggMesh->renderMeta.batchIndex;
	auto data = mm.solidBatches.at(bid).data[bi];
	auto dataSizeInBytes = mm.solidBatches.at(bid).dataSizeInBytes[bi];
	auto numVerts = dataSizeInBytes / (sizeof(float) * 3);*/

	/*float maxX, minX, maxY, minY, maxZ, minZ;

	maxX = FLT_MIN;
	minX = FLT_MAX;
	
	maxY = FLT_MIN;
	minY = FLT_MAX;

	maxZ = FLT_MIN;
	minZ = FLT_MAX;

	for (int i = 0; i < numVerts; i += 3)
	{
		glm::fvec3 vert;
		vert.x = data[i];
		vert.y = data[i+1];
		vert.z = data[i+2];
		maxX = vert.x > maxX ? vert.x : maxX;
		maxY = vert.y > maxY ? vert.y : maxY;
		maxZ = vert.z > maxZ ? vert.z : maxZ;

		minX = vert.x < minX ? vert.x : minX;
		minY = vert.y < minY ? vert.y : minY;
		minZ = vert.z < minZ ? vert.z : minZ;
	}

	glm::fvec3 origin((maxX + minX) / 2, (maxY + minY) / 2, (maxZ + minZ) / 2);

	float maxDist = 0;

	for (int i = 0; i < numVerts; i += 8)
	{
		glm::fvec3 vert;
		vert.x = data[i];
		vert.y = data[i + 1];
		vert.z = data[i + 2];

		float dist = glm::length(vert);
		maxDist = dist > maxDist ? dist : maxDist;
	}

	for (int i = 0; i < maxObjects; ++i)
	{
		cmds[i].count = mm.solidBatches[0].counts[0];
		cmds[i].instanceCount = 1;
		cmds[i].first = mm.solidBatches[0].firsts[0];
		cmds[i].radius = maxDist;
	}*/

	assets.prepareAsset(Asset::Type::Font, "res/fonts/clear-sans/ClearSans-Regular.ttf", "clearsans")->load();
	assets.prepareAsset(Asset::Type::Font, "res/fonts/clear-sans/ClearSans-Bold.ttf", "clearsansb")->load();

	cam.initialiseProj(float(window.getSizeX()) / float(window.getSizeY()));
	cam.calculateViewRays();
	r->initialiseRenderer(&window, cam);

	defaultOrthoCam.initaliseOrtho(window.getSizeX(), window.getSizeY());

	s64 tot = 0;

	float exposure = 1.f;

	createModelInfoWindow(uiw);

	world = new World();
	world->initialiseGLBuffers(1000);

	auto prevNode = world->getWorldRootNode();

	//for (int i = 0; i < 100; ++i)
	{
		//auto i1 = world->addMeshInstance(*a, prevNode);
		//prevNode = i1->sgNode;
		//i1->sgNode->transform.translate(glm::fvec3((i%100) * 500, 0, std::floor(float(i) / 100) * 500)).scale(100);
		//i1->sgNode->transform.translate(glm::fvec3(5, 0, 0));
		//i1->sgNode->transform.updateMatrix();
	}

	auto i2 = world->addMeshInstance(*b, world->getWorldRootNode());
	i2->sgNode->transform.translate(glm::fvec3(0, -50, 0));
	i2->sgNode->transform.updateMatrix();

	auto i3 = world->addMeshInstance(*ooMesh, world->getWorldRootNode());
	i3->sgNode->transform.translate(glm::fvec3(0, 20, 0)).scale(10);
	i3->sgNode->transform.updateMatrix();

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