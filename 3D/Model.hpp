#pragma once
#include "Asset.hpp"
#include "Include.hpp"
#include "boost/tokenizer.hpp"
#include "gli/load.hpp"
#include "SceneGraph.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "Texture.hpp"
#include "PhysicsObject.hpp"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct TextureMeta {
	String32 name;
	GLTexture* glTex;
	//s32 gpuIndex;
};

struct MaterialMeta {
	bool splatted;
	TextureMeta albedo, normal, specular;
};

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
		importModel();
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

	std::vector<TriangleList> triLists;

	
	s32 getPositionsArray(fltptr& arr)
	{
		s32 length = 0;
		for (auto list : triLists)
			length += list.numVerts;

		arr = new float[length];
		int j = 0, k = 0;

		for (auto list : triLists)
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

private:

	void importModel();

	void processNode(aiNode *node, const aiScene *scene);

	void processMesh(aiMesh *mesh, const aiScene *scene);

	void loadMaterialTextures(aiMaterial *material, aiTextureType type, TriangleList& triList);



};

class ModelInstance
{
public:

	void makePhysicsObject(btCollisionShape* collisionShape, float mass);

	Model* model;
	SGNode* sgNode;
	PhysicsObject* physicsObject;


};