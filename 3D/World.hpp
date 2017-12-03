#pragma once
#include "SceneGraph.hpp"
#include "Engine.hpp"
#include "AssetManager.hpp"
#include "BufferObjects.hpp"
#include "Renderer.hpp"
#include <unordered_map>
#include <map>

class World
{
public:
	World() { objectCount = 0; numTriLists[Regular] = 0; objectScopes.maxRegular = 1000; objectScopes.maxMultiTextured = 100;
	
	numTriLists[1] = 0;
	numTriLists[2] = 0;
	numTriLists[3] = 0;
	}
	~World() {}

	SGNode* getWorldRootNode()
	{
		return &sg.rootNode;
	}

	ModelInstance* addModelInstance(Model& model)
	{
		addModelInstance(model, &sg.rootNode);
	}

	ModelInstance* addModelInstance(Model& model, SGNode* parent)
	{
		++objectCount;

		u32 instanceID = numTriLists[Regular] + numTriLists[MultiTextured];
		numTriLists[Regular] += model.triLists.size();

		auto inst = modelInstances.insert(std::make_pair(instanceID, ModelInstance())).first;
		inst->second.model = &model;
		inst->second.sgNode = parent->addChild(SGNode());

		return &inst->second;
	}

	ModelInstance* addModelInstance(Model* model, SGNode* parent)
	{
		++objectCount;

		u32 instanceID = numTriLists[Regular] + numTriLists[MultiTextured];
		numTriLists[Regular] += model->triLists.size();

		auto inst = modelInstances.insert(std::make_pair(instanceID, ModelInstance())).first;
		inst->second.model = model;
		inst->second.sgNode = parent->addChild(SGNode());

		return &inst->second;
	}

	ModelInstance* addModelInstance(std::string modelName, SGNode* parent)
	{
		auto model = Engine::assets.getModel(String32(modelName.c_str()));

		++objectCount;

		u32 instanceID = numTriLists[Regular] + numTriLists[MultiTextured];
		numTriLists[Regular] += model->triLists.size();

		auto inst = modelInstances.insert(std::make_pair(instanceID, ModelInstance())).first;
		inst->second.setModel(model);
		inst->second.sgNode = parent->addChild(SGNode());

		return &inst->second;
	}

	ModelInstance* getModelInstance(u32 pInstanceID)
	{
		auto find = modelInstances.find(pInstanceID);
		while (find == modelInstances.end())
		{
			--pInstanceID;
			if (pInstanceID == -1)
				return nullptr;

			find = modelInstances.find(pInstanceID);
		}
		return &find->second;
	}

	void initialiseGLBuffers()
	{
		//instances.reserve(objectScopes.getTotalMaxObjects());

		auto maxRegular = objectScopes.maxRegular;
		auto maxMultiTextured = objectScopes.maxMultiTextured;

		texHandleBuffer[Regular].bufferData(maxRegular * sizeof(u64) * 6, 0, GL_STATIC_READ);
		drawIndirectBuffer[Regular].bufferData(maxRegular * sizeof(GLCMD), 0, GL_STREAM_DRAW);
		instanceTransformsBuffer[Regular].bufferData(maxRegular * sizeof(float) * 16, 0, GL_STATIC_READ);

		//texHandleBuffer[MultiTextured].bufferData(maxMultiTextured * sizeof(u64) * 13 , 0, GL_STATIC_READ);
		//drawIndirectBuffer[MultiTextured].bufferData(maxMultiTextured * sizeof(GLCMD), 0, GL_STREAM_DRAW);
		//instanceTransformsBuffer[MultiTextured].bufferData(maxMultiTextured * sizeof(float) * 16, 0, GL_STATIC_READ);

		drawIndirectBuffer[Shadow].bufferData((maxRegular + maxMultiTextured) * sizeof(GLCMD), 0, GL_STATIC_READ);
		instanceTransformsBuffer[Shadow].bufferData((maxRegular + maxMultiTextured) * sizeof(u32), 0, GL_STATIC_READ);

		//instanceIDBuffer.bufferData(objectScopes.getTotalMaxObjects() * sizeof(u32), 0, GL_DYNAMIC_READ);
	}

	/// TODO: updateInstanceRange, updateInstanceList

	void calculateGLMetrics()
	{
		glMetrics.regIndirectBufferSize = 0;
		glMetrics.shadowIndirectBufferSize = 0;

		for (auto mItr : modelInstances)
		{
			auto m = mItr.second.model;
			auto numTris = m->triLists.size();
			glMetrics.regIndirectBufferSize += numTris;
		}
	}

	void updateGLTransforms()
	{
		glm::fmat4* instanceTransformsRegular = (glm::fmat4*)instanceTransformsBuffer[Regular].mapRange(0, numTriLists[Regular] * sizeof(float) * 16, GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_WRITE_BIT);
		u32 i = 0;

		for (auto itr = modelInstances.begin(); itr != modelInstances.end(); ++itr)
		{
			auto model = itr->second.model;
			u32 lodLevel = 0;

			float camToModel = glm::length(itr->second.sgNode->transform.getTranslation() - Engine::r->activeCam->pos);

			for (auto it = model->lodLimits.begin(); it < model->lodLimits.end(); ++it)
			{
				if (camToModel < *it)
					break;
				else
					lodLevel++;
			}

			for (auto itr2 = itr->second.model->triLists[lodLevel].begin(); itr2 != itr->second.model->triLists[lodLevel].end(); ++itr2)
			{
				instanceTransformsRegular[i] = itr->second.sgNode->transform.getTransformMat();
				++i;
			}
		}

		instanceTransformsBuffer[Regular].unmap();
	}

	void updateGLBuffers()
	{
		GLCMD* indirectReg = (GLCMD*)drawIndirectBuffer[Regular].mapRange(0, numTriLists[Regular] * sizeof(GLCMD), GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_WRITE_BIT);
		glm::fmat4* instanceTransformsRegular = (glm::fmat4*)instanceTransformsBuffer[Regular].mapRange(0, numTriLists[Regular] * sizeof(float) * 16, GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_WRITE_BIT);
		auto texHandle = (u64*)texHandleBuffer[Regular].mapRange(0, numTriLists[Regular] * sizeof(u64) * 6, GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_WRITE_BIT);
		u32 i = 0;

		const GPUModelManager& mm = Engine::assets.modelManager;

		for (auto itr = modelInstances.begin(); itr != modelInstances.end(); ++itr)
		{
			auto model = itr->second.model;
			u32 lodLevel = 0;

			float camToModel = glm::length(itr->second.sgNode->transform.getTranslation() - Engine::r->activeCam->pos);

			for (auto i = model->lodLimits.begin(); i < model->lodLimits.end(); ++i)
			{
				if (camToModel < *i)
					break;
				else
					lodLevel++;
			}

			//while (lodLevel > model->lodLimits.size())
			//	lodLevel--;

			int triList = 0;

			for (auto itr2 = model->triLists[lodLevel].begin(); itr2 != model->triLists[lodLevel].end(); ++itr2)
			{
				auto bptr = itr2->renderMeta.batchPtr; //If render meta is missing, then most likely the mesh has not been pushed to a gpu batch
				auto bindex = itr2->renderMeta.batchIndex;

				indirectReg[i].count = bptr->counts[bindex];
				indirectReg[i].instanceCount = 1;
				indirectReg[i].first = bptr->firsts[bindex];
				indirectReg[i].baseInstance = 0;

				const auto& matOverwrite = itr->second.materialOverwrite[lodLevel][triList];

				if (matOverwrite.albedo.glTex)
					texHandle[(6 * i)] = matOverwrite.albedo.glTex->getHandle(Engine::r->defaultSampler.getGLID());
				else
					texHandle[(6 * i)] = itr2->matMeta.albedo.glTex->getHandle(Engine::r->defaultSampler.getGLID());

				if (matOverwrite.normal.glTex)
					texHandle[(6 * i) + 1] = matOverwrite.normal.glTex->getHandle(Engine::r->defaultSampler.getGLID());
				else
					texHandle[(6 * i) + 1] = itr2->matMeta.normal.glTex->getHandle(Engine::r->defaultSampler.getGLID());

				if (matOverwrite.specularMetallic.glTex)
					texHandle[(6 * i) + 2] = matOverwrite.specularMetallic.glTex->getHandle(Engine::r->defaultSampler.getGLID());
				else
					texHandle[(6 * i) + 2] = itr2->matMeta.specularMetallic.glTex->getHandle(Engine::r->defaultSampler.getGLID());

				if (itr2->matMeta.roughness.name.length() != 0)
					texHandle[(6 * i) + 3] = itr2->matMeta.roughness.glTex->getHandle(Engine::r->defaultSampler.getGLID());
				else
					texHandle[(6 * i) + 3] = 0;

				if (matOverwrite.roughness.glTex)
					texHandle[(6 * i) + 3] = matOverwrite.roughness.glTex->getHandle(Engine::r->defaultSampler.getGLID());

				instanceTransformsRegular[i] = itr->second.sgNode->transform.getTransformMat();

				++i;
				++triList;
			}
		}

		drawIndirectBuffer[Regular].unmap();
		instanceTransformsBuffer[Regular].unmap();
		texHandleBuffer[Regular].unmap();
		
		/*MeshGPUMetaMultiTextured* metaMultiTextured = (MeshGPUMetaMultiTextured*)objectMetaBuffer[MultiTextured].mapRange(0, numTriLists[MultiTextured] * sizeof(MeshGPUMetaMultiTextured), GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_WRITE_BIT);
		glm::fmat4* instanceTransformsMultiTextured = (glm::fmat4*)instanceTransformsBuffer[MultiTextured].mapRange(0, numTriLists[MultiTextured] * sizeof(float) * 16, GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_WRITE_BIT);
		i = 0;

		for (auto itr = instances.begin(); itr != instances.end(); ++itr)
		{
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
					s64 alphaHandle = (*itr3)->material.alpha->glData->getHandle(Engine::r->defaultSampler.getGLID());

					for (int j = 0; j < 4; ++j)
					{
						albedoHandles[j] = (*itr3)->material.albedo[j]->glData->getHandle(Engine::r->defaultSampler.getGLID());
						specularHandles[j] = (*itr3)->material.specular[j]->glData->getHandle(Engine::r->defaultSampler.getGLID());
						normalHandles[j] = (*itr3)->material.normal[j]->glData->getHandle(Engine::r->defaultSampler.getGLID());
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
		instanceTransformsBuffer[MultiTextured].unmap();*/
	}

	void updateDrawBuffer()
	{
		GLCMD* indirectReg = (GLCMD*)drawIndirectBuffer[Regular].mapRange(0, numTriLists[Regular] * sizeof(GLCMD), GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_WRITE_BIT);
		int i = 0;

		/// TODO: only update the LOD if the lod boundry was crossed. Do we need to map the whole range every time ?

		for (auto itr = modelInstances.begin(); itr != modelInstances.end(); ++itr)
		{
			auto model = itr->second.model;
			u32 lodLevel = 0;

			float camToModel = glm::length(itr->second.sgNode->transform.getTranslation() - Engine::r->activeCam->pos);

			for (auto it = model->lodLimits.begin(); it < model->lodLimits.end(); ++it)
			{
				if (camToModel < *it)
					break;
				else
					lodLevel++;
			}

			//while (lodLevel > model->lodLimits.size())
				//lodLevel--;

			for (auto itr2 = model->triLists[lodLevel].begin(); itr2 != model->triLists[lodLevel].end(); ++itr2)
			{
				auto bptr = itr2->renderMeta.batchPtr; //If render meta is missing, then most likely the mesh has not been pushed to a gpu batch
				auto bindex = itr2->renderMeta.batchIndex;

				indirectReg[i].count = bptr->counts[bindex];
				indirectReg[i].instanceCount = 1;
				indirectReg[i].first = bptr->firsts[bindex];
				indirectReg[i].baseInstance = 0;

				++i;
			}
		}

		drawIndirectBuffer[Regular].unmap();
		//instanceTransformsBuffer[Regular].unmap();
	}

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
		instanceTransformsBuffer :: array of transforms of each instance, 
			order should be determined by how dynamic an instance is(how much/often it moves) for ease of mapping/updating
	*/
	GLBufferObject instanceTransformsBuffer[DrawModesCount];


	/*
		instanceIDBuffer :: array of instance IDs of each visible instance
			re-generated by compute shader each frame
	*/
	GLBufferObject instanceIDBuffer;

	std::unordered_map<u32, ModelInstance> modelInstances;
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

	struct GLMetrics
	{
		s32 regIndirectBufferSize;
		s32 shadowIndirectBufferSize;
		
	} glMetrics;

	u32 objectCount;
	//u32 maxObjects;
	SceneGraph sg;
};