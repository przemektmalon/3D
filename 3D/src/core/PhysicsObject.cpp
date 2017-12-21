#include "PhysicsObject.hpp"
#include "Engine.hpp"
#include "AssetManager.hpp"

void PhysicsObject::updateGLAABB()
{
	btVector3 minbt, maxbt;
	collisionShape->getAabb(rigidBody->getWorldTransform(), minbt, maxbt);
	glm::fvec3 min(minbt.x(), minbt.y(), minbt.z()), max(maxbt.x(), maxbt.y(), maxbt.z());
	float verts[] = {
		min.x,			min.y,			min.z,
		max.x,			min.y,			min.z,
		max.x,			max.y,			min.z,
		min.x,			max.y,			min.z,

		min.x,			min.y,			max.z,
		min.x,			max.y,			max.z,
		max.x,			max.y,			max.z,
		max.x,			min.y,			max.z,

		min.x,			min.y,			min.z,
		min.x,			min.y,			max.z,
		min.x,			max.y,			max.z,
		min.x,			max.y,			min.z,

		max.x,			min.y,			min.z,
		max.x,			min.y,			max.z,
		max.x,			max.y,			max.z,
		max.x,			max.y,			min.z
	};
	memcpy(aabbLines, verts, sizeof(aabbLines));
	Engine::assets.modelManager.aabbBatch.vbo.bufferSubData(instance->id * sizeof(verts), sizeof(verts), verts);
}