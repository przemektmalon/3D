#pragma once
#include "Mesh.h"
#include "SceneGraph.h"
#include "Engine.h"
#include "AssetManager.h"
#include "BufferObjects.h"
#include "Renderer.h"
#include <unordered_map>

class World
{
public:
	World() { objectCount = 0; numTriLists[Regular] = 0; objectScopes.maxRegular = 1000; objectScopes.maxMultiTextured = 100; }
	~World() {}

	SGNode* getWorldRootNode()
	{
		return &sg.rootNode;
	}

	MeshInstance* addMeshInstance(Mesh& mesh, SGNode* parent)
	{
		++objectCount;

		MeshInstance mi;
		mi.sgNode = parent->addChild(SGNode());

		u32 instanceID = numTriLists[Regular] + numTriLists[MultiTextured];
		numTriLists[Regular] += mesh.getNumTriLists(Regular);
		numTriLists[MultiTextured] += mesh.getNumTriLists(MultiTextured);
		mi.mesh = &mesh;
		instances.insert(std::make_pair(instanceID, mi));
		return &instances.at(instanceID);
	}

	MeshInstance* getMeshInstance(u32 pInstanceID)
	{
		auto find = instances.find(pInstanceID);
		while (find == instances.end())
		{
			--pInstanceID;
			if (pInstanceID == -1)
				return nullptr;

			find = instances.find(pInstanceID);
		}
		return &find->second;
	}

	void initialiseGLBuffers()
	{
		//maxObjects = pMaxObjects;
		instances.reserve(objectScopes.getTotalMaxObjects());

		auto maxRegular = objectScopes.maxRegular;
		auto maxMultiTextured = objectScopes.maxMultiTextured;

		objectMetaBuffer[Regular].bufferData(maxRegular * sizeof(MeshGPUMetaRegular), 0, GL_STATIC_READ);
		texHandleBuffer[Regular].bufferData(maxRegular * sizeof(float) * 16, 0, GL_STATIC_READ); ///TODO: Check if 16 is correct ! Should be sizeof(u64) * numTextures.. ?
		drawIndirectBuffer[Regular].bufferData(maxRegular * sizeof(GLCMD), 0, GL_STREAM_DRAW);
		drawCountBuffer[Regular].bufferData(sizeof(u32), 0, GL_STREAM_READ);
		instanceTransformsBuffer[Regular].bufferData(maxRegular * sizeof(float) * 16, 0, GL_STATIC_READ);
		visibleTransformsBuffer[Regular].bufferData(maxRegular * sizeof(float) * 16, 0, GL_STATIC_READ);

		objectMetaBuffer[MultiTextured].bufferData(maxMultiTextured * sizeof(MeshGPUMetaMultiTextured), 0, GL_STATIC_READ);
		texHandleBuffer[MultiTextured].bufferData(maxMultiTextured * sizeof(glm::uvec4) * 7 , 0, GL_STATIC_READ);
		drawIndirectBuffer[MultiTextured].bufferData(maxMultiTextured * sizeof(GLCMD), 0, GL_STREAM_DRAW);
		drawCountBuffer[MultiTextured].bufferData(sizeof(u32), 0, GL_STREAM_READ);
		instanceTransformsBuffer[MultiTextured].bufferData(maxMultiTextured * sizeof(float) * 16, 0, GL_STATIC_READ);
		visibleTransformsBuffer[MultiTextured].bufferData(maxMultiTextured * sizeof(float) * 16, 0, GL_STATIC_READ);

		instanceIDBuffer.bufferData(objectScopes.getTotalMaxObjects() * sizeof(u32), 0, GL_DYNAMIC_READ);
	}

	//TODO: updateInstanceRange, updateInstanceList

	void updateGLBuffers()
	{
		MeshGPUMetaRegular* metaRegular = (MeshGPUMetaRegular*)objectMetaBuffer[Regular].mapRange(0, numTriLists[Regular] * sizeof(MeshGPUMetaRegular), GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_WRITE_BIT);
		glm::fmat4* instanceTransformsRegular = (glm::fmat4*)instanceTransformsBuffer[Regular].mapRange(0, numTriLists[Regular] * sizeof(float) * 16, GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_WRITE_BIT);
		u32 i = 0;

		const GPUMeshManager& mm = Engine::assets.meshManager;

		for (auto itr = instances.begin(); itr != instances.end(); ++itr)
		{
			auto numOverwriteMaterials = itr->second.overwriteMaterials.size();

			for (auto itr2 = itr->second.mesh->triangleListsSorted.begin(); itr2 != itr->second.mesh->triangleListsSorted.end(); ++itr2)
			{	
				for (auto itr3 = itr2->second.begin(); itr3 != itr2->second.end(); ++itr3)
				{
					if ((*itr3)->material.getDrawMode() != DrawMode::Regular)
						continue; ///TODO: Theres probably a better way to do this. Maybe sort by draw mode ! Instead of by materialID

					auto bptr = (*itr3)->renderMeta.batchPtr;
					auto bindex = (*itr3)->renderMeta.batchIndex;

					metaRegular[i].cmds[0] = bptr->counts[bindex];
					metaRegular[i].cmds[1] = 1;
					metaRegular[i].cmds[2] = bptr->firsts[bindex];

					s64 albedoHandle = (*itr3)->material.albedo[0]->getHandle(Engine::r->defaultSampler.getGLID());
					s64 normalHandle = (*itr3)->material.normal[0]->getHandle(Engine::r->defaultSampler.getGLID());

					metaRegular[i].albedoHandle_RadiusYZ.r = (u32)(albedoHandle);
					metaRegular[i].albedoHandle_RadiusYZ.g = (u32)(albedoHandle >> 32);

					metaRegular[i].normal_bump_Handles.r = (u32)(normalHandle);
					metaRegular[i].normal_bump_Handles.g = (u32)(normalHandle >> 32);

					metaRegular[i].albedoHandle_RadiusYZ.b = bptr->radii[bindex].y;
					metaRegular[i].albedoHandle_RadiusYZ.a = bptr->radii[bindex].z;
					metaRegular[i].radiusX = bptr->radii[bindex].x;

					instanceTransformsRegular[i] = itr->second.sgNode->transform.getTransformMat();

					++i;
				}
			}
		}

		objectMetaBuffer[Regular].unmap();
		instanceTransformsBuffer[Regular].unmap();
		
		MeshGPUMetaMultiTextured* metaMultiTextured = (MeshGPUMetaMultiTextured*)objectMetaBuffer[MultiTextured].mapRange(0, numTriLists[MultiTextured] * sizeof(MeshGPUMetaMultiTextured), GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_WRITE_BIT);
		glm::fmat4* instanceTransformsMultiTextured = (glm::fmat4*)instanceTransformsBuffer[MultiTextured].mapRange(0, numTriLists[MultiTextured] * sizeof(float) * 16, GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_WRITE_BIT);
		i = 0;

		for (auto itr = instances.begin(); itr != instances.end(); ++itr)
		{
			auto numOverwriteMaterials = itr->second.overwriteMaterials.size();

			for (auto itr2 = itr->second.mesh->triangleListsSorted.begin(); itr2 != itr->second.mesh->triangleListsSorted.end(); ++itr2)
			{
				for (auto itr3 = itr2->second.begin(); itr3 != itr2->second.end(); ++itr3)
				{
					if ((*itr3)->material.getDrawMode() != DrawMode::MultiTextured)
						continue;

					auto bptr = (*itr3)->renderMeta.batchPtr;
					auto bindex = (*itr3)->renderMeta.batchIndex;

					metaMultiTextured[i].cmds[0] = bptr->counts[bindex];
					metaMultiTextured[i].cmds[1] = 1;
					metaMultiTextured[i].cmds[2] = bptr->firsts[bindex];

					s64 albedoHandles[4];
					s64 specularHandles[4];
					s64 normalHandles[4];
					s64 alphaHandle = (*itr3)->material.alpha->getHandle(Engine::r->defaultSampler.getGLID());

					for (int j = 0; j < 4; ++j)
					{
						albedoHandles[j] = (*itr3)->material.albedo[j]->getHandle(Engine::r->defaultSampler.getGLID());
						specularHandles[j] = (*itr3)->material.specular[j]->getHandle(Engine::r->defaultSampler.getGLID());
						normalHandles[j] = (*itr3)->material.normal[j]->getHandle(Engine::r->defaultSampler.getGLID());
					}

					metaMultiTextured[i].albedoHandleA_RadiusYZ.r = (u32)(albedoHandles[0]);
					metaMultiTextured[i].albedoHandleA_RadiusYZ.g = (u32)(albedoHandles[0] >> 32);

					metaMultiTextured[i].specularA_normalA.r = (u32)(specularHandles[0]);
					metaMultiTextured[i].specularA_normalA.g = (u32)(specularHandles[0] >> 32);

					metaMultiTextured[i].specularA_normalA.b = (u32)(normalHandles[0]);
					metaMultiTextured[i].specularA_normalA.a = (u32)(normalHandles[0] >> 32);

					metaMultiTextured[i].albedoB_specularB.r = (u32)(albedoHandles[1]);
					metaMultiTextured[i].albedoB_specularB.g = (u32)(albedoHandles[1] >> 32);

					metaMultiTextured[i].albedoB_specularB.b = (u32)(specularHandles[1]);
					metaMultiTextured[i].albedoB_specularB.a = (u32)(specularHandles[1] >> 32);

					metaMultiTextured[i].normalB_albedoC.r = (u32)(normalHandles[1]);
					metaMultiTextured[i].normalB_albedoC.g = (u32)(normalHandles[1] >> 32);

					metaMultiTextured[i].normalB_albedoC.b = (u32)(albedoHandles[2]);
					metaMultiTextured[i].normalB_albedoC.a = (u32)(albedoHandles[2] >> 32);

					metaMultiTextured[i].specularC_normalC.r = (u32)(specularHandles[2]);
					metaMultiTextured[i].specularC_normalC.g = (u32)(specularHandles[2] >> 32);

					metaMultiTextured[i].specularC_normalC.b = (u32)(normalHandles[2]);
					metaMultiTextured[i].specularC_normalC.a = (u32)(normalHandles[2] >> 32);

					metaMultiTextured[i].albedoD_specularD.r = (u32)(albedoHandles[3]);
					metaMultiTextured[i].albedoD_specularD.g = (u32)(albedoHandles[3] >> 32);

					metaMultiTextured[i].albedoD_specularD.b = (u32)(specularHandles[3]);
					metaMultiTextured[i].albedoD_specularD.a = (u32)(specularHandles[3] >> 32);

					metaMultiTextured[i].normalD_alpha.r = (u32)(normalHandles[3]);
					metaMultiTextured[i].normalD_alpha.g = (u32)(normalHandles[3] >> 32);

					metaMultiTextured[i].normalD_alpha.b = (u32)(alphaHandle);
					metaMultiTextured[i].normalD_alpha.a = (u32)(alphaHandle >> 32);

					metaMultiTextured[i].albedoHandleA_RadiusYZ.b = bptr->radii[bindex].y;
					metaMultiTextured[i].albedoHandleA_RadiusYZ.a = bptr->radii[bindex].z;
					//metaMultiTextured[i].alphaScale = (*itr3)->material.alphaScale;
					metaMultiTextured[i].alphaScale = 1.f / 50.f;

					instanceTransformsMultiTextured[i] = itr->second.sgNode->transform.getTransformMat();

					++i;
				}
			}
		}

		objectMetaBuffer[MultiTextured].unmap();
		instanceTransformsBuffer[MultiTextured].unmap();
	}

	

	/*
		objectMetaBuffer :: array of structs "MeshGPUMetaRegular"
			would need to be updated for every texture change
	*/
	GLBufferObject objectMetaBuffer[DrawModesCount];
	/*
		texHandleBuffer :: array of visible instance tex handles, 
			re-generated by compute shader each frame
	*/
	GLBufferObject texHandleBuffer[DrawModesCount];
	/*
		drawIndirectBuffer :: array of indirect draw commands, 
			re-generated by compute shader each frame
	*/
	GLBufferObject drawIndirectBuffer[DrawModesCount];
	/*
		drawCountBuffer :: one u32 value, represents number of instances visible in current frame
			re-generated by compute shader each frame
	*/
	GLBufferObject drawCountBuffer[DrawModesCount];
	/*
		instanceTransformsBuffer :: array of transforms of each instance, 
			order should be determined by how dynamic an instance is(how much/often it moves) for ease of mapping/updating
	*/
	GLBufferObject instanceTransformsBuffer[DrawModesCount];
	/*
		visibleTransformsBuffer :: array of transforms of each visible instance,
			re-generated by compute shader each frame
	*/
	GLBufferObject visibleTransformsBuffer[DrawModesCount];
	/*
		instanceIDBuffer :: array of instance IDs of each visible instance
			re-generated by compute shader each frame
	*/
	GLBufferObject instanceIDBuffer;

	std::unordered_map<u32, MeshInstance> instances;
	u32 numTriLists[DrawModesCount];

	struct ObjectScopes
	{
		ObjectScopes() : maxRegular(0), maxMultiTextured(0) {}
		ObjectScopes(u32 pMaxRegular, u32 pMaxMultiTextured) : maxRegular(pMaxRegular), maxMultiTextured(pMaxMultiTextured) {}
		u32 maxRegular;
		u32 maxMultiTextured;

		u32 getTotalMaxObjects() { return maxRegular + maxMultiTextured; }
	};

	ObjectScopes objectScopes;

	u32 objectCount;
	//u32 maxObjects;
	SceneGraph sg;
};