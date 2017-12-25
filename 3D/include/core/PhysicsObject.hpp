#pragma once

#include <Windows.h>
#include "btBulletDynamicsCommon.h"
#include "MathIncludes.hpp"
#include "BufferObjects.hpp"
#include "GLIncludes.hpp"

#undef min
#undef max

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

		updateGLAABB();
	}

	void setMass(float pMass)
	{
		mass = pMass;
		collisionShape->calculateLocalInertia(mass, inertia);
		rigidBody->setMassProps(mass, inertia);
	}

	void setFriction(float f)
	{
		rigidBody->setFriction(f);
	}

	void setRestitution(float r)
	{
		rigidBody->setRestitution(r);
	}

	void setDamping(float linear, float angular)
	{
		rigidBody->setDamping(linear, angular);
	}

	void setLinearDamping(float linear)
	{
		rigidBody->setDamping(linear, rigidBody->getAngularDamping());
	}

	void setAngularDamping(float angular)
	{
		rigidBody->setDamping(rigidBody->getLinearDamping(), angular);
	}

	void updateGLAABB();

	btRigidBody* rigidBody;
	btCollisionShape* collisionShape;
	btDefaultMotionState* motionState;
	btVector3 inertia;
	btScalar mass;

	ModelInstance* instance;
	float aabbLines[48];
};