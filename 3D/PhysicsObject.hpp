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

		setFriction(0.5f);
		setRestitution(0.5f);
		setDamping(0.05, 0.05);
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

	btRigidBody* rigidBody;
	btCollisionShape* collisionShape;
	btDefaultMotionState* motionState;
	btVector3 inertia;
	btScalar mass;

	ModelInstance* instance;
};