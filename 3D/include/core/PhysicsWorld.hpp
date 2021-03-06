#pragma once
#include <unordered_map>
#include "PhysicsObject.hpp"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"
#include <vector>
#include "Time.hpp"

class ModelInstance;

class PhysicsWorld
{
public:
	PhysicsWorld() {}
	~PhysicsWorld() 
	{
		delete dynamicsWorld;
		delete solver;
		delete dispatcher;
		delete collisionConfiguration;
		delete broadphase;
	}

	void create()
	{
		broadphase = new btDbvtBroadphase();
		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collisionConfiguration);
		solver = new btSequentialImpulseConstraintSolver();

		btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);

		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

		dynamicsWorld->setGravity(btVector3(0, -2, 0));
	}

	void createGroundPlane();

	void addRigidBody(PhysicsObject* body)
	{
		dynamicsWorld->addRigidBody(body->rigidBody);
		objects.push_back(body);
	}

	void removeRigidBody(PhysicsObject* body)
	{
		dynamicsWorld->removeRigidBody(body->rigidBody);

	}

	void step(Time& dt)
	{
		dynamicsWorld->stepSimulation(dt.getMilliSecondsf(), 10, 1.f/60.f);
	}

	void updateModels();

	btVector3 getRayTo(int x, int y);
	bool pickBody(btVector3& rayFromWorld, btVector3& rayToWorld);
	void removePickingConstraint();
	bool movePickedBody(const btVector3& rayFromWorld, const btVector3& rayToWorld);
	bool mouseMoveCallback(int x, int y);

	btBroadphaseInterface* broadphase;

	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;

	btDiscreteDynamicsWorld* dynamicsWorld;

	std::vector<PhysicsObject*> objects;

	int savedState;
	btRigidBody* pickedBody;
	btPoint2PointConstraint* p2p;
	btPoint2PointConstraint* pickedConstraint;

	btVector3 oldPickingPos;
	btVector3 hitPos;
	btScalar oldPickingDist;

};
