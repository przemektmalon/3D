#pragma once
#include "SceneGraph.hpp"
#include "BufferObjects.hpp"
#include <unordered_map>
#include <map>
#include "DrawModes.hpp"
#include "Texture.hpp"
#include "Model.hpp"
#include "TextBillboard.hpp"

class Model;

class World
{
public:
	World();
	~World() {}

	SGNode* getWorldRootNode();

	ModelInstance* addModelInstance(Model* model, SGNode* parent);
	ModelInstance* addModelInstance(Model& model, SGNode* parent);
	ModelInstance* addModelInstance(std::string modelName, SGNode* parent);
	ModelInstance* addModelInstance(Model& model);
	ModelInstance* addModelInstance(std::string modelName);

	ModelInstance* getModelInstance(u32 pInstanceID);

	void initialiseGLBuffers();

	/// TODO: updateInstanceRange, updateInstanceList

	void calculateGLMetrics();

	void updateGLTransforms();
	void updateGLBuffers();
	void updateDrawBuffer();

	void setSkybox(std::string pathToFolder);

	/*
		texHandleBuffer :: array of visible instance tex handles, 
	*/
	GLBufferObject texHandleBuffer[DrawModesCount];

	/*
		drawIndirectBuffer :: array of indirect draw commands, 
	*/
	GLBufferObject drawIndirectBuffer[DrawModesCount];

	/*
		aabbIndirectBuffer :: array of indirect draw commands,
	*/
	GLBufferObject aabbIndirectBuffer;

	/*
		aabbTransformsBuffer :: array of aabb transforms,
	*/
	GLBufferObject aabbTransformsBuffer;

	/*
		instanceTransformsBuffer :: array of transforms of each instance, 
			order should be determined by how dynamic an instance is(how much/often it moves) for ease of mapping/updating
	*/
	GLBufferObject instanceTransformsBuffer[DrawModesCount];

	std::unordered_map<u32, ModelInstance> modelInstances;
	std::unordered_map<u32, TextBillboard> tags;
	u32 numTriLists[DrawModesCount];

	GLTextureCube skybox;

	struct ObjectScopes
	{
		ObjectScopes() : maxRegular(0), maxMultiTextured(0) {}
		ObjectScopes(u32 pMaxRegular, u32 pMaxMultiTextured) : maxRegular(pMaxRegular), maxMultiTextured(pMaxMultiTextured) {}
		u32 maxRegular;
		u32 maxMultiTextured;

		u32 getTotalMaxObjects() { return maxRegular + maxMultiTextured; }
	};

	ObjectScopes objectScopes;

	struct GLMetrics
	{
		s32 regIndirectBufferSize;
		s32 shadowIndirectBufferSize;
		
	} glMetrics;

	u32 objectCount;
	//u32 maxObjects;
	SceneGraph sg;
};