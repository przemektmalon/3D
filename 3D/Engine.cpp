#pragma once
#include "Engine.hpp"
#include "Include.hpp"
#include "Shader.hpp"
#include "Time.hpp"
#include "QPC.hpp"
#include "SOIL.h"
#include "Camera.hpp"
#include "Renderer.hpp"
#include <chrono>
#include "Font.hpp"
#include "Text.hpp"
#include <windows.h>
#include <strsafe.h>
#include <functional>
#include "AssetManager.hpp"
#include "Billboard.hpp"

#include "glm\gtc\quaternion.hpp"

#define MODEL_PATH std::string("res/model/")

#include "World.hpp"

#include "StringGenerics.hpp"

#include "UILabel.hpp"
#include "UIButton.hpp"

#include "ModelInfo.hpp"


#include "Console.hpp"

#include "Tweaks.hpp"


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
Physics Engine::p;
PhysicsWorld Engine::physics;

EngineConfig Engine::cfg;

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

	Engine::r->tileCullShader.use();
	Engine::r->tileCullShader.setSelectedID(selectedID);
	Engine::r->tileCullShader.sendSelectedID();
	Engine::r->tileCullShader.stop();

	delete[] idTex;
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
	//Engine::r->tileCullShader.reload();
	Engine::r->ssaoShader.reload();
}

void killFocus()
{
	Engine::window.postMessage(WM_KILLFOCUS, 0, 0);
}

void toggleConsole()
{
	Engine::console.toggle();
}

btVector3 getRayTo(int x, int y)
{

	//x -= Engine::window.getSizeX() / 2;
	//y -= Engine::window.getSizeY() / 2;

	float ndcx = (2.0f * x) / float(Engine::window.getSizeX()) - 1.0f;
	float ndcy = 1.0f - (2.0f * y) / float(Engine::window.getSizeY());
	float ndcz = 1.0f;
	//vec3 ray_nds = vec3(x, y, z);

	glm::fvec4 ray_clip(ndcx, ndcy, -1.f, 1.f);

	glm::fvec4 ray_eye;

	ray_eye = Engine::r->activeCam->inverseProj * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.f, 0.0);

	glm::fvec4 w = (glm::inverse(Engine::r->activeCam->view) * ray_eye);
	glm::fvec3 ray_wor(w.x, w.y, w.z);

	ray_wor = glm::normalize(ray_wor) * 10000.f;

	//std::cout << "RAY_WOR: " << ray_wor.x << " " << ray_wor.y << " " << ray_wor.z << std::endl;

	return btVector3(ray_wor.x, ray_wor.y, ray_wor.z);
}

bool pickBody(btVector3& rayFromWorld, btVector3& rayToWorld)
{
	if (Engine::physics.dynamicsWorld == 0)
		return false;

	static int i = 0;

	rayToWorld = rayFromWorld + (rayToWorld*10000.f);

	btCollisionWorld::ClosestRayResultCallback rayCallback(rayFromWorld, rayToWorld);
	Engine::physics.dynamicsWorld->rayTest(rayFromWorld, rayToWorld, rayCallback);
	if (rayCallback.hasHit()) {
		btVector3 pickPos = rayCallback.m_hitPointWorld;
		//btRigidBody* body = (btRigidBody*)btRigidBody::upcast(rayCallback.m_collisionObject);
		btRigidBody* body = (btRigidBody*)btRigidBody::upcast(rayCallback.m_collisionObject);
		if (body) {
			if (!(body->isStaticObject() || body->isKinematicObject())) 
			{
				Engine::p.pickedBody = body;
				Engine::p.savedState = Engine::p.pickedBody->getActivationState();
				Engine::p.pickedBody->setActivationState(DISABLE_DEACTIVATION);

				//std::cout << "NEW CONSTRAINT " << ++i << std::endl;

				btVector3 localPivot = body->getCenterOfMassTransform().inverse() * pickPos;
				Engine::p.p2p = new btPoint2PointConstraint(*body, localPivot);
				Engine::physics.dynamicsWorld->addConstraint(Engine::p.p2p, true);
				Engine::p.pickedConstraint = Engine::p.p2p;
				btScalar mousePickClamping = 3000.f;
				Engine::p.p2p->m_setting.m_impulseClamp = mousePickClamping;
				Engine::p.p2p->m_setting.m_tau = 0.01f;
			}
		}
		Engine::p.oldPickingPos = rayToWorld;
		Engine::p.hitPos = pickPos;
		Engine::p.oldPickingDist = (pickPos - rayFromWorld).length();
	}
}

void removePickingConstraint() {
	if (Engine::p.pickedConstraint) {
		Engine::p.pickedBody->forceActivationState(Engine::p.savedState);
		Engine::p.pickedBody->activate();
		Engine::physics.dynamicsWorld->removeConstraint(Engine::p.pickedConstraint);
		delete Engine::p.pickedConstraint;
		Engine::p.pickedConstraint = 0;
		Engine::p.pickedBody = 0;
	}
}

bool movePickedBody(const btVector3& rayFromWorld, const btVector3& rayToWorld) {
	if (Engine::p.pickedBody  && Engine::p.pickedConstraint) {
		btPoint2PointConstraint* pickCon = static_cast<btPoint2PointConstraint*>(Engine::p.pickedConstraint);
		if (pickCon) {
			btVector3 newPivotB;
			btVector3 dir = rayToWorld - rayFromWorld;
			dir.normalize();
			dir *= Engine::p.oldPickingDist;
			newPivotB = rayFromWorld + dir;
			pickCon->setPivotB(newPivotB);
			return true;
		}
	}
	return false;
}

bool mouseMoveCallback(float x, float y)
{
	btVector3 rayTo = getRayTo(int(x), int(y));
	glm::fvec3 p = Engine::r->activeCam->pos;
	btVector3 rayFrom(p.x, p.y, p.z);
	
	movePickedBody(rayFrom, rayTo);

	return false;
}

void Engine::mainLoop()
{
	glewExperimental = GL_TRUE;
	glewInit();
	//wglewInit();
	
	wglSwapIntervalEXT(0);

	uim.mapToKeyDown(VK_ESCAPE, escapePress);
	uim.mapToKeyDown('P', setRes);
	uim.mapToKeyDown('L', screenshot);
	uim.mapToKeyDown('O', toggleWireFrame);
	uim.mapToKeyDown('I', toggleTextBounds);

	uim.mapToKeyDown('M', hotLoadShader);

	uim.mapToKeyDown(VK_OEM_3, toggleConsole);

	uim.mapToKeyDown('K', printlog);
	uim.mapToKeyDown('Q', killFocus);

	uim.mapToMouseDown(0, mouseDown);
	uim.mapToMouseUp(0, mouseUp);

	r = new MasterRenderer();

	physics.create();

	rand.seed(rand.default_seed);

	startTime = std::chrono::system_clock::now().time_since_epoch().count();

	FT_Init_FreeType(&ftLib);

	r->initialiseShaders();
	Billboard::initGLVAO();

	assets.loader.loadAssets(String128("res/resources.txt"));
	assets.pushTexturesToGPU();

	assets.modelManager.init();

	auto m = assets.prepareModel(String128("res/model/COLBOX.dae"), String32("colbox"));
	m->load();
	assets.modelManager.pushModelToBatch(*m);

	auto m2 = assets.prepareModel(String128("res/model/ground2.obj"), String32("ground"));
	m2->load();
	assets.modelManager.pushModelToBatch(*m2);

	auto m3 = assets.prepareModel(String128("res/model/mon.obj"), String32("mon"));
	m3->load();
	assets.modelManager.pushModelToBatch(*m3);

	const GPUModelManager& mm = Engine::assets.modelManager;

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

	auto c1 = new btStaticPlaneShape(glm::fvec3(0.f, 1.f, 0.f), 0.f);
	auto c2 = new btStaticPlaneShape(btVector3(1.f, 0.f, 0.f), -50.f);
	auto c3 = new btStaticPlaneShape(btVector3(-1.f, 0.f, 0.f), -50.f);
	auto c4 = new btStaticPlaneShape(btVector3(0.f, 0.f, 1.f), -50.f);
	auto c5 = new btStaticPlaneShape(btVector3(0.f, 0.f, -1.f), -50.f);
	auto c6 = new btStaticPlaneShape(btVector3(0.f, -1.f, 0.f), -100.f);

	auto trans = btTransform(btMatrix3x3(1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f));

	auto s = new btCompoundShape();
	s->addChildShape(trans, c1);
	s->addChildShape(trans, c2);
	s->addChildShape(trans, c3);
	s->addChildShape(trans, c4);
	s->addChildShape(trans, c5);
	s->addChildShape(trans, c6);


	auto worldRoot = world->getWorldRootNode();

	auto ee = assets.getModel(String32("ground"));
	auto i8 = world->addModelInstance(*ee, worldRoot);
	i8->sgNode->transform.scale(1.f);
	auto col = new btStaticPlaneShape(glm::fvec3(0.f, 1.f, 0.f), 0.f);
	i8->makePhysicsObject(s, 0.f);
	
	i8->physicsObject->rigidBody->setRestitution(0.95f);

	ee = assets.getModel(String32("colbox"));
	auto i7 = world->addModelInstance(*ee, worldRoot);
	i7->sgNode->transform.scale(8.f);
	i7->sgNode->transform.translate(glm::fvec3(0, 50, 0));
	auto boxcol = new btBoxShape(glm::fvec3(8, 8, 8));
	i7->makePhysicsObject(boxcol, 42.f);
	
	i7->physicsObject->rigidBody->setRestitution(0.4);

	auto i = world->addModelInstance(*ee, worldRoot);
	i->sgNode->transform.scale(glm::fvec3(4, 6,3));
	i->sgNode->transform.translate(glm::fvec3(10, 50, 0));
	boxcol = new btBoxShape(glm::fvec3(4, 6, 3));
	i->makePhysicsObject(boxcol, 30.f);

	i->physicsObject->rigidBody->setRestitution(0.4);

	ee = assets.getModel(String32("mon"));

	for (int i = 0; i < 4; ++i)
	{
		auto i2 = world->addModelInstance(*ee, worldRoot);
		i2->sgNode->transform.scale(10.f);
		i2->sgNode->transform.translate(glm::fvec3(9 * i, 70, 0));
		auto scol = new btSphereShape(10);

		i2->makePhysicsObject(scol, 50.f);
		i2->physicsObject->rigidBody->setRestitution(0.95);
	}

	world->sg.updateAll();

	world->updateGLBuffers();
	r->world = world;

	console.init();

	Time t; t.setSeconds(1.f);

	Tweaks tweak;
	tweak.setTweaksFile("res/tweaks.txt");
	tweak.setUpdateTime(t);
	tweak.bindVariable(cfg.world.camSpeed, "camSpeed", Tweaks::Floating);

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

					if (ev.mouse.code.code == MouseCode::M_LEFT)
					{
						glm::fvec3 p = r->activeCam->pos;
						btVector3 camPos(p.x, p.y, p.z);

						btVector3 rayFrom = camPos;
						btVector3 rayTo = getRayTo(int(ev.mouse.position.x), int(ev.mouse.position.y));

						pickBody(rayFrom, rayTo);
					}

					break;
				}
				case(Event::MouseUp):
				{
					uim.mouseUp(ev.mouse.code);
					uiw->mouseUp((MouseEvent&)ev);

					removePickingConstraint();

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
			ev.constructMouse(MouseCode::Code::M_NONE, window.getMousePosition(), 0);
			uiw->mouseHover((MouseEvent&)ev);
			mouseMoveCallback(ev.mouse.position.x, ev.mouse.position.y);

			if(console.stateFlags == 0)
				uim.keyHolds(window.keyboard);

			switch (engineState)
			{
			case(InGame):
			{
				tweak.updateTweaks();
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

	auto keyboardState = window.keyboard.keyState;

	auto move = glm::fvec3(glm::fvec4(0, 0, cfg.world.camSpeed * dt.getSeconds(), 1) * cam.matYaw);
	cam.targetPos -= move * float(window.keyboard.isKeyPressed('W'));

	move = glm::cross(glm::fvec3(glm::fvec4(0, 0, cfg.world.camSpeed * dt.getSeconds(), 1) * cam.matYaw), glm::fvec3(0, 1, 0));
	cam.targetPos += move * float(window.keyboard.isKeyPressed('A'));

	move = glm::fvec3(glm::fvec4(0, 0, cfg.world.camSpeed * dt.getSeconds(), 1) * cam.matYaw);
	cam.targetPos += move * float(window.keyboard.isKeyPressed('S'));

	move = glm::cross(glm::fvec3(glm::fvec4(0, 0, cfg.world.camSpeed * dt.getSeconds(), 1) * cam.matYaw), glm::fvec3(0, 1, 0));
	cam.targetPos -= move * float(window.keyboard.isKeyPressed('D'));

	cam.targetPos.y += cfg.world.camSpeed * dt.getSeconds() * float(window.keyboard.isKeyPressed('R'));

	cam.targetPos.y -= cfg.world.camSpeed * dt.getSeconds() * float(window.keyboard.isKeyPressed('F'));

	static float exposure = 1.f;

	physics.step();
	physics.updateModels();

	if (window.keyboard.isKeyPressed('I'))
	{
		auto rad = r->ssaoShader.getRadius();
		auto newRad = rad;
		newRad += 0.1 * dt.getSecondsf();
		newRad = std::max(newRad, 0.0000000001f);

		r->ssaoShader.setRadius(newRad);

		std::cout << "Rad = " << newRad << std::endl;
	}
	if (window.keyboard.isKeyPressed('U'))
	{
		auto rad = r->ssaoShader.getRadius();
		auto newRad = rad;
		newRad -= 0.1 * dt.getSecondsf();
		newRad = std::max(newRad, 0.0000000001f);

		r->ssaoShader.setRadius(newRad);

		std::cout << "Rad = " << newRad << std::endl;
	}

	if (window.keyboard.isKeyPressed('9'))
	{
		auto ints = r->ssaoShader.getIntensity();
		std::cout << ints << std::endl;
		auto newInts = ints;
		newInts += 5 * dt.getSecondsf();
		newInts = std::max(newInts, 0.0001f);

		r->ssaoShader.setIntensity(newInts);
	}
	if (window.keyboard.isKeyPressed('8'))
	{
		auto ints = r->ssaoShader.getIntensity();
		auto newInts = ints;
		newInts -= 5 * dt.getSecondsf();
		newInts = std::max(newInts, 0.0001f);

		r->ssaoShader.setIntensity(newInts);
	}

	uiw->update();
	cam.update(dt);

	r->render();

	dt.setMicroSeconds(qpc.getElapsedTime());
	programTime += dt.getSecondsf();
}

void Engine::processMenuFrame()
{

}