#include "PhysicsWorld.hpp"
#include "Engine.hpp"
#include "World.hpp"

void PhysicsWorld::updateModels()
{
	btTransform t;
	for (auto o : objects)
	{
		o->rigidBody->getMotionState()->getWorldTransform(t);
		btQuaternion q = t.getRotation();
		btVector3 p = t.getOrigin();
		o->instance->sgNode->transform.setQuat(glm::fquat(q.w(),q.x(),q.y(),q.z()));
		o->instance->sgNode->transform.setTranslation(glm::fvec3(p.x(),p.y(),p.z()));
	}
	Engine::world->sg.updateAll();
	Engine::world->updateGLTransforms();
}