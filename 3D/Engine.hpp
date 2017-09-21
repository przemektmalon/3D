#pragma once

#include "Window.hpp"
#include "Shader.hpp"
#include <random>
#include "UserInputManager.hpp"
#include "Logger.hpp"
#include "UIWindow.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H;

#include "PhysicsWorld.hpp"

class FontStore;
class Mesh;
class Camera;
class Time;
class MasterRenderer;
class QPC;
class AssetManager;
class World;
class Console;
class MeshUtility;

class Physics
{
public:

	Physics()
	{

	}

	~Physics()
	{
		delete dynamicsWorld;
		delete solver;
		delete dispatcher;
		delete collisionConfiguration;
		delete broadphase;
	}

	btBroadphaseInterface* broadphase;

	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;

	btDiscreteDynamicsWorld* dynamicsWorld;

	btCollisionShape* groundShape;
	btCollisionShape* fallShape;

	btDefaultMotionState* groundMotionState;

	btRigidBody* groundRigidBody;
	btRigidBody* fallRigidBody;

	btRigidBody* pickedBody;

	int savedState;
	btPoint2PointConstraint* p2p;
	btPoint2PointConstraint* pickedConstraint;

	btVector3 oldPickingPos;
	btVector3 hitPos;
	btScalar oldPickingDist;
};

class Engine
{
public:
	Engine() {}
	~Engine() {}

	static void start(HINSTANCE pHInstance);
	static void mainLoop();
	static void processGameFrame();
	static void processMenuFrame();
	static void stop();
	static void select(glm::ivec2 mPos);

	static World* world;

	static float programTime;

	static std::mt19937_64 rand;

	static Camera defaultOrthoCam;

	static FT_Library ftLib;
	static s64 startTime;

	static Console console;
	static bool movingLight;
	static Camera cam;
	static Time dt;
	static MasterRenderer* r;
	static QPC qpc;
	static glm::ivec2 lastM;
	static UIM uim;
	static Logger logger;

	static s32 selectedID;

	static UIWindow* uiw;
	static bool windowClicked;
	static glm::ivec2 clickedPos;
	static Log log;
	static MeshUtility mu;

	static AssetManager assets;

	static bool consoleOpen;

	static Physics p;
	static PhysicsWorld physics;

	static float moveSpeed;
	static float radDiff;

//private:

	enum EngineState { InGame, Menu, Quitting };

	static EngineState engineState;
	static Window window;

	static HINSTANCE instance;
	static char workingDirectory[MAX_PATH];
	static u32 workingDirectoryLength;
};