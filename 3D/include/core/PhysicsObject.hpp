#pragma once

#include "btBulletDynamicsCommon.h"
#include "MathIncludes.hpp"
#include "BufferObjects.hpp"

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

	void updateGLAABB()
	{
		btVector3 minbt, maxbt;
		collisionShape->getAabb(rigidBody->getWorldTransform(), minbt, maxbt);
		glm::fvec3 min(minbt.x(), minbt.y(), minbt.z()), max(maxbt.x(), maxbt.y(), maxbt.z());
		glm::fvec3 size(max.x - min.x, max.y - min.y, max.z - min.z);
		float verts[] = {
			min.x,			min.y,			min.z,
			min.x + size.x, min.y,			min.z,
			min.x + size.x, min.y + size.y, min.z,
			min.x,			min.y + size.y, min.z,

			min.x,			min.y + size.y, min.z + size.z,
			min.x + size.x, min.y + size.y, min.z + size.z,
			min.x + size.x, min.y,			min.z + size.z,
			min.x,			min.y,			min.z + size.z,

			min.x,			min.y,			min.z,
			min.x + size.x, min.y,			min.z,
			min.x + size.x, min.y,			min.z + size.z,
			min.x + size.x, min.y + size.y,	min.z + size.z,
			min.x + size.x,	min.y + size.y,	min.z
		};

		//aabb.bufferData(sizeof(verts), verts, GL_STREAM_DRAW);
	}

	btRigidBody* rigidBody;
	btCollisionShape* collisionShape;
	btDefaultMotionState* motionState;
	btVector3 inertia;
	btScalar mass;

	ModelInstance* instance;
};