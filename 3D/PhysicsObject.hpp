#pragma once

#include "btBulletDynamicsCommon.h"
#include "MathIncludes.hpp"

class ModelInstance;

class PhysicsObject
{
public:

	void create(glm::fvec3 pos, glm::fquat rot, btCollisionShape* shape, float pMass)
	{
		mass = pMass;
		collisionShape = shape;

		motionState = new btDefaultMotionState(btTransform(btQuaternion(rot.x, rot.y, rot.z, rot.w), btVector3(pos.x, pos.y, pos.z)));

		collisionShape->calculateLocalInertia(mass, inertia);

		rigidBody = new btRigidBody(mass, motionState, collisionShape, inertia);
	}

	btRigidBody* rigidBody;
	btCollisionShape* collisionShape;
	btDefaultMotionState* motionState;
	btVector3 inertia;
	btScalar mass;
	ModelInstance* instance;
};