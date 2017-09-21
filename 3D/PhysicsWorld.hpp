#pragma once

#include "PhysicsObject.hpp"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/Gimpact/btGImpactCollisionAlgorithm.h"
#include <vector>

class PhysicsWorld
{
public:

	void create()
	{
		broadphase = new btDbvtBroadphase();
		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collisionConfiguration);
		solver = new btSequentialImpulseConstraintSolver();

		btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);

		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

		dynamicsWorld->setGravity(btVector3(0, -20, 0));
	}

	void addRigidBody(PhysicsObject* body)
	{
		dynamicsWorld->addRigidBody(body->rigidBody);
		objects.push_back(body);
	}

	void removeRigidBody(PhysicsObject* body)
	{
		dynamicsWorld->removeRigidBody(body->rigidBody);

	}

	void step()
	{
		dynamicsWorld->stepSimulation(1.f / 60.f, 10);
	}

	void updateModels();

	btBroadphaseInterface* broadphase;

	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;

	btDiscreteDynamicsWorld* dynamicsWorld;

	std::vector<PhysicsObject*> objects;
};