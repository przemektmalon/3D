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
#include "FontStore.h"
#include <windows.h>
#include <strsafe.h>
//#include "SDL_syswm.h"
#include <functional>
#include "resource.h"
#include "ResourceManager.h"

#define MODEL_PATH std::string("res/model/")

#include "UILabel.h"

FT_Library Engine::ftLib;
//FontStore Engine::fontStore;
Camera Engine::defaultOrthoCam;
//Text Engine::t;
HINSTANCE Engine::instance;
Shader Engine::s;
Shader Engine::testShader;
Shader Engine::gPassShader;
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
u32 Engine::selectedID;
UIWindow* Engine::uiw;
bool Engine::windowClicked = false;
glm::ivec2 Engine::clickedPos;
ResourceManager Engine::resMan;

/*int main(int argc, char *argv[])
{
Engine::start();
return 0;
}*/


MasterRenderer* asd;

Window Engine::window;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd)
{
	MSG msg = { 0 };
	WNDCLASS wc = { 0 };

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

	window.createWindow(instance, "Engine", 0, 0, 1920, 1080, ws);
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

	while (glGetError());

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, idTex);

	u32 source = r->rC.renderResolution.x * (r->rC.renderResolution.y - 1) + (-1.f * (mPos.y * r->rC.renderResolution.x) + (mPos.x));
	auto index = source;

	selectedID = idTex[index];

	if (selectedID == 0)
	{
		selectedID = -1;
	}

	Engine::r->tileCullShader.use();
	Engine::r->tileCullShader.setSelectedID(selectedID);
	Engine::r->tileCullShader.stop();

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
	Engine::r->ents[Engine::selectedID - 1].transform.translate(glm::fvec3(Engine::dt.getSeconds() * 50, 0, 0));
}

void moveObjXN()
{
	Engine::r->ents[Engine::selectedID - 1].transform.translate(glm::fvec3(Engine::dt.getSeconds() * -50, 0, 0));
}

void moveObjZP()
{
	Engine::r->ents[Engine::selectedID - 1].transform.translate(glm::fvec3(0, 0, Engine::dt.getSeconds() * 50));
}

void moveObjZN()
{
	Engine::r->ents[Engine::selectedID - 1].transform.translate(glm::fvec3(0, 0, Engine::dt.getSeconds() * -50));
}

void moveObjYP()
{
	Engine::r->ents[Engine::selectedID - 1].transform.translate(glm::fvec3(0, Engine::dt.getSeconds() * 50, 0));
}

void moveObjYN()
{
	Engine::r->ents[Engine::selectedID - 1].transform.translate(glm::fvec3(0, Engine::dt.getSeconds() * -50, 0));
}

void mouseDown()
{
	auto mPos = Engine::window.mouse.getWindowPosition(&Engine::window);
	auto uiw = Engine::uiw;
	UIRect wa = uiw->getWindowArea();
	if (mPos.x > wa.left && mPos.x < wa.left + wa.width && mPos.y > wa.top && mPos.y < wa.top + wa.height)
	{
		Engine::windowClicked = true;
		Engine::clickedPos = mPos - glm::ivec2(wa.left, wa.top);
	}
}

void mouseUp()
{
	Engine::windowClicked = false;
}

void Engine::mainLoop()
{
	glewExperimental = GL_TRUE;
	glewInit();

	uim.mapToKeyDown(VK_ESCAPE, escapePress);
	uim.mapToKeyDown('P', setRes);
	uim.mapToKeyDown('L', screenshot);
	uim.mapToKeyDown('O', toggleWF);
	uim.mapToKeyDown(VK_OEM_COMMA, loadPosition);
	uim.mapToKeyDown(VK_OEM_PERIOD, savePosition);

	uim.mapToKeyHold(VK_NUMPAD2, moveObjZP);
	uim.mapToKeyHold(VK_NUMPAD8, moveObjZN);
	uim.mapToKeyHold(VK_NUMPAD6, moveObjXP);
	uim.mapToKeyHold(VK_NUMPAD4, moveObjXN);
	uim.mapToKeyHold(VK_NUMPAD9, moveObjYP);
	uim.mapToKeyHold(VK_NUMPAD7, moveObjYN);

	uim.mapToMouseDown(0, mouseDown);
	uim.mapToMouseUp(0, mouseUp);

	r = new MasterRenderer();

	rand.seed(rand.default_seed);

	startTime = std::chrono::system_clock::now().time_since_epoch().count();

	FT_Init_FreeType(&ftLib);
	//fontStore.initialise();

	//Font font;
	//font.load(size, "gor.ttf", fc);

	//t.init();
	//t.setFont(&font);
	//t.setCharSize(size);
	//t.setString("Hello!");

	s.load("res/shader/Standard", "res/shader/Standard");
	gPassShader.load("res/shader/gBufferPass", "res/shader/gBufferPass");
	testShader.load("res/shader/test", "res/shader/test");
	glUseProgram(s());
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

	//glViewport(0, 0, window.getSizeX(), window.getSizeY());

	resMan.init();

	char b1[] = "res/model/box.bin";
	char b1n[] = "box";

	char b2[] = "res/model/box2.bin";
	char b2n[] = "box2";

	char b3[] = "res/model/box3.bin";
	char b3n[] = "box3";

	auto a = resMan.registerMesh(b1, b1n);
	auto b = resMan.registerMesh(b2, b2n);
	auto c = resMan.registerMesh(b3, b3n);

	resMan.loadMesh(a);
	resMan.loadMesh(b);
	resMan.loadMesh(c);

	GLTexture2DMip t1;
	GLTexture2DMip t2;
	GLTexture2DMip t3;
	GLTexture2DMip t4;

	t1.createFromFile("res/tex/g.jpg");
	t2.createFromFile("res/tex/gS.jpg");
	t3.createFromFile("res/tex/gN.jpg");
	t4.createFromFile("res/tex/gB.jpg");

	/*std::string fileName("wd.jpg");
	int width, height;
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	unsigned char* image = SOIL_load_image(fileName.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);

	fileName = std::string("wd.jpg");
	GLuint specularMap;
	glGenTextures(1, &specularMap);
	glBindTexture(GL_TEXTURE_2D, specularMap);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	image = SOIL_load_image(fileName.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);

	fileName = std::string("m.png");
	GLuint marble;
	glGenTextures(1, &marble);
	glBindTexture(GL_TEXTURE_2D, marble);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	image = SOIL_load_image(fileName.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);*/

	std::vector<glm::fmat4> inst;
	for (int i = 0; i < 1; ++i)
	{
		auto in = glm::translate(glm::fmat4(), glm::fvec3(i * 150, 30, 0));
		in = glm::scale(in, glm::fvec3(4.f));
		in = glm::rotate(in, float(PI) / 5.f, glm::fvec3(0, 1.f, 0));
		inst.push_back(in);
	}
	//MasterRenderer r;
	//r.entities.insert(std::make_pair(&buddha, inst));
	//std::vector<glm::fmat4> inst2;
	//for (int i = 0; i < 10; ++i)
	//{
	//	auto in = glm::translate(glm::fmat4(), glm::fvec3(i * 200, 35, -150));
	//	in = glm::scale(in, glm::fvec3(10.f));
	//	in = glm::rotate(in, float(PI) / 4.f, glm::fvec3(0, 1, 0));
	//	inst2.push_back(in);
	//}
	//r->entities.insert(std::make_pair(&dragon, inst2));
	//MeshInstance m1;
	//m1.meshData = &dragon;
	//m1.transform = Transform().scale(10);
	//m1.texx.createFromFile("m.png");
	//r->ents.push_back(m1);
	//MeshInstance m2;
	//m2.meshData = &sp;
	//m2.transform = Transform().translate(glm::fvec3(100, 0, 0)).scale(10);
	//m2.texx.createFromFile("res/sp.jpg");
	//r->ents.push_back(m2);
	//MeshInstance m3;
	//m3.meshData = &oo;
	//m3.transform = Transform().translate(glm::fvec3(200, 0, 0)).scale(10);
	//m3.texx.createFromFile("res/oo.jpg");
	//r->ents.push_back(m3);
	//MeshInstance m4;
	//m4.meshData = &pf;
	//m4.transform = Transform().translate(glm::fvec3(-200, 0, 0)).scale(20);
	//m4.texx.createFromFile("res/pf.jpg");
	//r->ents.push_back(m4);
	//MeshInstance m5;
	//m5.meshData = &ter;
	//m5.transform = Transform().scale(400);
	//m5.texx.createFromFile("res/tex/stone.png");
	//m5.specTex.createFromFile("res/tex/stone.png");
	//m5.normalTex.createFromFile("res/tex/stoneN.png");
	//m5.texx.createFromFile("res/tex/g.jpg");
	//m5.specTex.createFromFile("res/tex/gS.jpg");
	//m5.normalTex.createFromFile("res/tex/gN.jpg");
	//m5.bumpTex.createFromFile("res/tex/gB.jpg");
	//r->ents.push_back(m5);
	//std::vector<glm::fmat4> inst3;
	//inst3.push_back(glm::scale(glm::translate(glm::fmat4(), glm::fvec3(10, -10, 10)), glm::fvec3(50)));
	//r->entities.insert(std::make_pair(&floor, inst3));
	//std::vector<glm::fmat4> inst4;
	//inst4.push_back(glm::scale(glm::translate(glm::fmat4(), glm::fvec3(-30, -5, 70)), glm::fvec3(0.8)));
	//r.entities.insert(std::make_pair(&ballpyr, inst4));
	//std::vector<glm::fmat4> inst5;
	//inst5.push_back(glm::rotate(glm::scale(glm::translate(glm::fmat4(), glm::fvec3(0, -88, 0)), glm::fvec3(50)), glm::radians(-32.f), glm::fvec3(0, 1, 0)));
	//r.entities.insert(std::make_pair(&table, inst5));

	std::vector<glm::fmat4> inst4;
	inst4.push_back(glm::scale(glm::translate(glm::fmat4(), glm::fvec3(0, 100, 0)), glm::fvec3(10)));

	//r->entities.insert(std::make_pair(&sp, inst4));
	//sp.tex.createFromFile("res/sp.jpg");

	inst4[0] = glm::translate(inst4[0], glm::fvec3(22, 0, 0));

	//r->entities.insert(std::make_pair(&oo, inst4));
	//oo.tex.createFromFile("res/oo.jpg");

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, textureID);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);

	//Camera cam;
	cam.initialiseProj(float(window.getSizeX()) / float(window.getSizeY()));
	cam.calculateViewRays();
	r->initialiseRenderer(&window, cam);

	defaultOrthoCam.initaliseOrtho(window.getSizeX(), window.getSizeY());

	s64 tot = 0;

	glm::ivec2 mousePosAtClick;
	bool warpedToMidAfterClick;
	bool mouseRightDown;

	auto expval = glGetUniformLocation(s(), "exposure");
	float exposure = 1.f;

	std::string stringg;

	Font font;
	font.load("res/fonts/clear-sans/ClearSans-Regular.ttf");
	font.loadGlyphs(16);

	UILabel lbl;
	lbl.text.init();
	lbl.text.setFont(&font);
	lbl.text.setCharSize(16);
	lbl.text.setWindowSize(glm::ivec2(500, 500));
	lbl.text.setWindowOrigin(Text2D::TopLeft);
	lbl.text.setString("Resolution: 1920x1080");
	lbl.text.setTextOrigin(glm::ivec2(0, 16));
	lbl.text.setPosition(glm::fvec2(20, 50));
	lbl.text.setColour(glm::fvec3(0.95f, 0.95f, 0.95f));

	uiw = new UIWindow(UIRect(0, 0, 500, 500), 6, &window);

	uiw->setTitle("DEBUG OPTIONS");
	uiw->addElement(&lbl);

	while (engineState != Quitting) {
		if (!window.processMessages())
		{
			window.keyboard.updateKeyState();
			uim.keyHolds(window.keyboard);
			processFrame();
		}
	}
	int x = 10;
};

void Engine::processFrame()
{
	qpc.start();

	/*while (SDL_PollEvent(&ev))
	{
	if (ev.type == SDL_KEYDOWN)
	{
	if (ev.key.keysym.sym == SDLK_BACKSPACE)
	{
	if (stringg.size() > 0)
	{
	stringg.assign(stringg.begin(), stringg.end() - 1);
	//t.setString(stringg);
	}
	}
	if (ev.key.keysym.sym == SDLK_ESCAPE)
	{
	quit = true;
	}
	if (ev.key.keysym.sym == SDLK_KP_1)
	{
	wf = true;
	}
	if (ev.key.keysym.sym == SDLK_KP_2)
	{
	wf = false;
	}
	if (ev.key.keysym.sym == SDLK_KP_4)
	{
	movingLight = true;
	}
	if (ev.key.keysym.sym == SDLK_KP_5)
	{
	movingLight = false;
	}
	if (ev.key.keysym.sym == SDLK_q)
	{
	glUseProgram(Engine::s());
	auto val = glGetUniformLocation(s(), "filmic");
	glUniform1i(val, 0);
	}
	if (ev.key.keysym.sym == SDLK_e)
	{
	glUseProgram(Engine::s());
	auto val = glGetUniformLocation(s(), "filmic");
	glUniform1i(val, 1);
	}
	if (ev.key.keysym.sym == SDLK_PERIOD)
	{
	window.screenshot();
	}
	}
	if (ev.type == SDL_WINDOWEVENT)
	{
	if (ev.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
	{
	std::cout << "FOCUS GAINED" << std::endl;
	int x, y;

	//SetCursorPos(window.getSizeX() / 2, window.getSizeY() / 2);

	hasFocus = true;
	//SDL_ShowCursor(1);
	}
	if (ev.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
	{
	std::cout << "FOCUS LOST" << std::endl;
	hasFocus = false;
	//SDL_ShowCursor(1);
	}
	}
	if (ev.type == SDL_MOUSEBUTTONDOWN)
	{
	if (ev.button.button == SDL_BUTTON_RIGHT)
	{
	mousePosAtClick = glm::ivec2(ev.button.x, ev.button.y);
	//SDL_WarpMouseInWindow(window.sdlWindow, window.sizeX / 2, window.sizeY / 2);

	//SetCursorPos(window.sizeX / 2, window.sizeY / 2);

	POINT p;
	GetCursorPos(&p);

	std::cout << "0. " << p.x << " " << p.y << std::endl;
	warpedToMidAfterClick = true;
	lastM = glm::ivec2(p.x, p.y);
	mouseRightDown = true;
	//SDL_GetRelativeMouseState(NULL, NULL);
	//SDL_ShowCursor(0);
	}
	}
	if (ev.type == SDL_MOUSEBUTTONUP)
	{
	if (ev.button.button == SDL_BUTTON_RIGHT)
	{
	mouseRightDown = false;
	lastM = glm::ivec2(window.sizeX / 2, window.sizeY / 2);
	//SDL_WarpMouseInWindow(window.sdlWindow, mousePosAtClick.x, mousePosAtClick.y);
	//SDL_ShowCursor(1);
	}
	}
	if (ev.type == SDL_MOUSEMOTION)
	{
	u32 mask = SDL_GetWindowFlags(window.sdlWindow);
	if (((mask & SDL_WINDOW_INPUT_FOCUS) == SDL_WINDOW_INPUT_FOCUS) && mouseRightDown)
	{
	POINT m;
	GetCursorPos(&m);
	//auto m = SDL_GetMouseState(&x, &y);
	//auto m = SDL_GetRelativeMouseState(&x, &y);



	glm::vec2 mouse_delta = glm::vec2(m.x, m.y) - glm::vec2(lastM);
	//glm::ivec2 mouse_delta = glm::ivec2(m.x, m.y) - glm::ivec2(window.sizeX / 2, window.sizeY / 2);
	lastM = glm::ivec2(m.x, m.y);

	std::cout << "1. " << mouse_delta.x << " " << mouse_delta.y << std::endl;
	std::cout << "2. " << m.x << " " << m.y << std::endl << std::endl;

	const float mouseX_Sensitivity = 0.004f;
	const float mouseY_Sensitivity = 0.004f;

	cam.targetYaw += mouseX_Sensitivity * mouse_delta.x;
	cam.targetPitch += mouseY_Sensitivity * mouse_delta.y;

	//SDL_WarpMouseInWindow(window.sdlWindow, window.sizeX / 2, window.sizeY / 2);
	//SetCursorPos(window.sizeX / 2, window.sizeY / 2);
	}
	}
	if (ev.type == SDL_TEXTINPUT)
	{
	stringg += ev.text.text[0];
	//t.setString(stringg);
	}
	}*/

	if (window.mouse.rightDown)
	{
		SetCursorPos(window.getPosX() + (window.getSizeX() / 2), window.getPosY() + (window.getSizeY() / 2));
	}

	if (windowClicked)
	{
		//uiw->drag(window.mouse.getDelta());
		uiw->setWindowPosition(window.mouse.getWindowPosition(&window) - clickedPos);
	}

	const float moveSpeed = 400;

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

	if (window.keyboard.isKeyPressed('I'))
	{
		r->defaultSampler.setTextureAnisotropy(1);
	}
	if (window.keyboard.isKeyPressed('U'))
	{
		r->defaultSampler.setTextureAnisotropy(16);
	}

	/*exposure += float(keyboardState['X']) * dt.getSeconds();
	exposure -= float(keyboardState['Z']) * dt.getSeconds();
	exposure = max(0.f, exposure);
	s.use();
	glUniform1f(expval, exposure);
	glUniform1f(expval, exposure);
	s.stop();*/

	r->ssaoShader.setIntensity(r->ssaoShader.intensity + (float(keyboardState['B']) * 5 * dt.getSeconds()));
	r->ssaoShader.setIntensity(r->ssaoShader.intensity - (float(keyboardState['V']) * 5 * dt.getSeconds()));
	r->ssaoShader.intensity = max(0.f, r->ssaoShader.intensity);

	r->ssaoShader.setRadius(r->ssaoShader.radius + (float(keyboardState['H']) * 5 * dt.getSeconds()));
	r->ssaoShader.setRadius(r->ssaoShader.radius - (float(keyboardState['G']) * 5 * dt.getSeconds()));
	r->ssaoShader.radius = max(0.f, r->ssaoShader.radius);

	cam.update(dt);

	int textureID = 0, specularMap = 1, marble = 2;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, marble);

	r->render();

	dt.setMicroSeconds(qpc.getElapsedTime());
}