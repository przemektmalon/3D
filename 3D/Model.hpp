#pragma once
#include "Asset.hpp"
#include "Include.hpp"
#include "boost/tokenizer.hpp"
#include "SceneGraph.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "Texture.hpp"
#include "PhysicsObject.hpp"



#include "Shader.hpp"
#include "Transform.hpp"
#include "Engine.hpp"

#include "Bounds3D.hpp"

#include "File.hpp"

#include <string>

#include "Material.hpp"

class MeshBatch;

class ModelRenderMeta
{
public:
	MeshBatch* batchPtr;
	u32 batchIndex;
};

class Model : public Asset
{
	typedef float* fltptr;
public:
	Model() { type = Mesh; }
	Model(String128& pPath, String32& pName) : Asset(pPath, pName) { type = Mesh; }
	~Model() {}

	void save(StringGeneric& binPath) {}

	void load() 
	{
		if (lodLimits.size() != 0)
			assert(lodPaths.size() == lodLimits.size());
		else
			lodLimits.push_back(INT_MAX);

		for (int i = 0; i < lodPaths.size(); ++i)
		{
			importModel(lodPaths[i], i);
		}


	}

	struct TriangleList {
		TriangleList(float* pData, u32 pSize, u32 pNumVerts) {
			data = pData;
			size = pSize;
			numVerts = pNumVerts;
		}

		float* data;
		u32 size;
		u32 numVerts;

		MaterialMeta matMeta;
		ModelRenderMeta renderMeta;

		s32 getDataSizeInBytes()
		{
			return size * sizeof(float);
		}

		s32 getDataSizeInFloats()
		{
			return size;
		}
	};

	std::vector<std::string> lodPaths;
	std::vector<u32> lodLimits;
	std::vector<std::vector<TriangleList>> triLists;

	s32 getPositionsArray(fltptr& arr, u32 lod)
	{
		s32 length = 0;
		for (auto list : triLists[lod])
			length += list.numVerts;

		arr = new float[length];
		int j = 0, k = 0;

		for (auto list : triLists[lod])
		{
			for (int i = 0; i < list.numVerts; i += 8)
			{
				arr[j] = list.data[i]; ++j;
				arr[j] = list.data[i + 1]; ++j;
				arr[j] = list.data[i + 2]; ++j;
			}
		}
		return length;
	}

	std::string physicsInfo;

private:

	void importModel(std::string pPath, u32 lod);

	void processNode(aiNode *node, const aiScene *scene, u32 lod);

	void processMesh(aiMesh *mesh, const aiScene *scene, std::vector<TriangleList>& triListVec);

	void loadMaterialTextures(aiMaterial *material, aiTextureType type, TriangleList& triList);



};

class ModelInstance
{
public:

	void setModel(Model* m)
	{
		materialOverwrite.clear();
		model = m;
		for (int i = 0; i < model->triLists.size(); ++i)
		{
			materialOverwrite.push_back(std::vector<MaterialMeta>());
			for (int j = 0; j < model->triLists[i].size(); ++j)
			{
				materialOverwrite.back().push_back(MaterialMeta());
			}
		}
	}

	void makePhysicsObject(btCollisionShape* collisionShape, float mass);
	void makePhysicsObject();

	void overwriteMaterial(int lod, int triList, MaterialMeta& material)
	{
		if (materialOverwrite.size() < lod)
			return; /// TODO: log ?

		auto& lodTris = materialOverwrite[lod];

		if (lodTris.size() < triList)
			return; /// TODO: log ?

		lodTris[triList] = material;
	}

	Model* model;
	SGNode* sgNode;
	PhysicsObject* physicsObject;

	std::vector<std::vector<MaterialMeta>> materialOverwrite;
};