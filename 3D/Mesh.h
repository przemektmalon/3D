#pragma once
#include "Include.h"
#include <string>
#include <fstream>
#include "Shader.h"
#include "Transform.h"
#include "Engine.h"
#include "Asset.h"
#include "SceneGraph.h"

#include "Texture.h"

#include "Bounds3D.h"

struct ObjectData
{
	std::vector<float> verts;
	std::vector<float> texCoords;
	std::vector<float> normals;
	std::vector<int> indices;
	s32 numVert;
	s32 numTris;
};

struct InterlacedObjectData
{
	float* interlacedData;
	s32 size;
};

class VertexAttrib
{
public:

	enum VertexAttribType {
		flt2, flt3, flt4,
		dbl2, dbl3, dbl4,
		AttribTypesCount
	};

	static const int typeSizes[AttribTypesCount];
	static const String<16> typeNames[AttribTypesCount];

	String<32> name;
	VertexAttribType type;

	void operator=(VertexAttrib& rhs)
	{
		name.overwrite(rhs.name);
		type = rhs.type;
	}
};

class VertexFormat
{
public:

	VertexAttrib attribs[12];
	s32 numAttribs;
	s32 size;

	s32 calculateSizeInBytes()
	{
		size = 0;
		for (int i = 0; i < numAttribs; ++i)
		{
			size += VertexAttrib::typeSizes[attribs[i].type];
		}
		return size;
	}

	void operator=(VertexFormat& rhs)
	{
		for (int i = 0; i < 12; ++i)
		{
			attribs[i] = rhs.attribs[i];
		}
		numAttribs = rhs.numAttribs;
		size = rhs.size;
	}

	void defineGLVertexAttribs(GLuint vao);

};

enum MaterialID
{
	PN,
	PNUU_T_S_N,
	PNUU_TT_SS_NN,
	PNUU_TT_SS,
	PNUU_TT_NN,
	PNUU_TTT_SSS_NNN,
	MATERIALS_COUNT
};

class Material
{
public:
	Material() {}

	MaterialID matID;

	GLTexture2D* albedo[4];
	GLTexture2D* specular[4];
	GLTexture2D* normal[4];

	static VertexFormat vertexFormats[MATERIALS_COUNT];
	static void initVertexFormats()
	{
		VertexFormat* vf = vertexFormats + PNUU_T_S_N;
		vf->numAttribs = 3;
		vf->attribs[0].name.setToChars("p");
		vf->attribs[0].type = VertexAttrib::flt3;
		vf->attribs[1].name.setToChars("n");
		vf->attribs[1].type = VertexAttrib::flt3;
		vf->attribs[2].name.setToChars("t");
		vf->attribs[2].type = VertexAttrib::flt2;
		vf->calculateSizeInBytes();
	}
};

class OBJMeshData;

class InterleavedVertexData
{
public:
	InterleavedVertexData() : data(nullptr) {}
	~InterleavedVertexData()
	{
		if (data != nullptr)
		{
			delete[] data;
		}
	}

	void fromOBJMeshData(OBJMeshData* obj);

	s32 getDataSizeInBytes() { return size * sizeof(float); }

	VertexFormat format;
	float* data;
	s32 size;

};

class MeshBatch;

class MeshRenderMeta
{
public:
	//u32 batchID;
	MeshBatch* batchPtr;
	u32 batchIndex;
};

struct MeshInstanceMeta
{
	MeshRenderMeta renderMeta;
	SGNode* nodePtr;
};

class Mesh : public Asset
{
	friend class GPUMeshManager;
	friend class MeshUtility;
	friend class World;
public:
	Mesh() {}
	Mesh(String128& pPath, String32& pName) : Asset(pPath, pName) {}
	~Mesh() 
	{
		for (int i = 0; i < triangleLists.size(); ++i)
		{
			delete[] triangleLists[i].data;
		}
	}

	void loadBinary()
	{
		/*std::ifstream ifs(diskPath.getString(), std::ios_base::binary);
		s32 size;
		float* glVerts;
		ifs.read((char*)&size, sizeof(size));
		intData.interlacedData = (new float[size]);
		ifs.read((char*)intData.interlacedData, sizeof(GLfloat) * size);
		intData.size = size;
		data.numVert = size / 8;
		data.numTris = data.numVert / 3;*/
	}

	void saveBinary(StringGeneric& binPath)
	{
		/*std::ofstream ofs(binPath.getString(), std::ios_base::binary);
		ofs.write((char*)&intData.size, sizeof(intData.size));
		std::cout << intData.size << std::endl;
		ofs.write((char*)intData.interlacedData, intData.size * sizeof(float));*/
	}

	void saveBinV10(StringGeneric& binPath)
	{
		std::ofstream ofs(binPath.getString(), std::ios_base::binary);
		char major = 1; char minor = 0;
		ofs.write(&major, sizeof(major));
		ofs.write(&minor, sizeof(minor));
		ofs.write(getName().getString(), getName().getCapacity());
		auto numTriLists = triangleLists.size();
		ofs.write((char*)&(numTriLists), sizeof(numTriLists));

		for (auto i = 0; i < triangleLists.size(); ++i)
		{
			TriangleList* tl = &triangleLists[i];
			ofs.write((char*)&tl->material.matID, sizeof(tl->material.matID));
			ofs.write((char*)&tl->numVerts, sizeof(tl->numVerts));
			ofs.write(tl->material.albedo[0]->getName().getString(), tl->material.albedo[0]->getName().getCapacity());
			auto size = tl->getDataSizeInBytes();
			ofs.write((char*)&size, sizeof(size));
			ofs.write((char*)tl->data, size);
		}
	}

	void load()
	{
		loadBinV10();
		sortTriangleLists();
	}

	void loadBinV10();

	void sortTriangleLists()
	{
		for (auto itr = triangleLists.begin(); itr != triangleLists.end(); ++itr)
		{
			bool added = false;
			for (auto itr2 = triangleListsSorted.begin(); itr2 != triangleListsSorted.end(); ++itr2)
			{
				if (itr2->first == itr->material.matID)
				{
					itr2->second.push_back(&(*itr));
					added = true;
					break;
				}
			}
			if (!added)
			{
				triangleListsSorted.push_back(std::make_pair(itr->material.matID, std::vector<Mesh::TriangleList*>(1, &(*itr))));
			}
		}
	}

	//ObjectData data;
	//InterlacedObjectData intData;
	MeshRenderMeta renderMeta;
	//InterleavedVertexData ivd;
	//OBJMeshData* objMeshData;

	struct TriangleList
	{
		TriangleList() {}

		Material material;
		s32 numVerts;
		s32 first;
		float* data;

		MeshRenderMeta renderMeta;

		void copyConstruct(TriangleList& tl)
		{
			data = new float[tl.numVerts * tl.material.vertexFormats->size];
			numVerts = tl.numVerts;
			material = tl.material;
			first = tl.first;
			memcpy(data, tl.data, tl.numVerts * tl.material.vertexFormats->calculateSizeInBytes());
		}

		s32 getDataSizeInBytes()
		{
			return numVerts * material.vertexFormats[material.matID].size;
		}
	};

	u32 getNumGPUTriLists()
	{
		u32 ret = 0;
		for (auto itr = triangleListsSorted.begin(); itr != triangleListsSorted.end(); ++itr)
		{
			///TODO: THIS ISNT THE ONLY MATERIAL THE GPU SHOULD TAKE
			if (itr->first == PNUU_T_S_N)
			{
				ret++;
			}
		}
		return ret;
	}

//private:

	std::vector<TriangleList> triangleLists;
	std::vector<std::pair<MaterialID, std::vector<TriangleList*>>> triangleListsSorted;
	std::vector<String<32>> listTextureNames;

	AABounds3D bounds;
	float radius;
};

struct MeshGPUMeta
{
	MeshGPUMeta() {}
	union
	{
		glm::uvec4 cmds;
		struct
		{
			glm::uvec3 cmdss;
			float radius;
		};
	};
	glm::uvec4 texHandleMatID;
	glm::uvec4 normalBumpMap;
};

class MeshInstance
{
public:
	MeshInstance() {}
	~MeshInstance() {}

//private:
	Mesh* mesh;
	//u32 instanceID;
	MeshRenderMeta renderMeta;
	SGNode* sgNode;
	//GLTexture2DMip texx;
	//GLTexture2DMip specTex;
	//GLTexture2DMip normalTex;
	//GLTexture2DMip bumpTex;
};

#define MAX_BATCH_COUNT 512
#define MAX_BATCH_SIZE 1024*1024*64

