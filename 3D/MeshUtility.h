#pragma once
#include "Mesh.h"
#include "Strings.h"
#include "AssetManager.h"

class SeperatedVertexData
{
public:
	SeperatedVertexData()
	{
		for (int i = 0; i < 12; ++i)
		{
			data[i] = nullptr;
		}
	}
	~SeperatedVertexData()
	{
	}

	VertexFormat format;
	void* data[12];
};

class OBJMeshData
{
public:
	struct Group
	{
		Group(String64& name) : groupName(name) {}
		String64 groupName;
		String64 materialName;
		
		std::vector<int> indices;

		int getNumVerts()
		{
			return indices.size() / 3;
		}

		int getNumTris()
		{
			return getNumVerts() / 3;
		}

	};

	SeperatedVertexData vData;
	std::vector<Group> groups;
	String<32> name;
	String<128> path;
	std::map<String64,String32> materials;

	int getNumVerts() 
	{ 
		int ret = 0;
		for (auto itr = groups.begin(); itr != groups.end(); ++itr)
		{
			ret += itr->indices.size() / 3;
		}
		return ret;
	}
	int getNumTris() 
	{ 
		return getNumVerts() / 3; 
	}

	void loadMY(StringGeneric& pPath);
	void load(StringGeneric& pPath);
};

class MeshUtility
{
public:
	MeshUtility() {}
	~MeshUtility() {}

	void loadOBJ(String<128>& pPath, s32 objIndex);

	void loadBin(String<128>& pPath, s32 binIndex)
	{
		if (binIndex > meshes.size() - 1)
			return;

		auto mesh = meshes[binIndex];
		if (mesh == nullptr)
		{
			mesh = new Mesh(pPath,String<32>("MESH"));
		}
		else
		{
			delete mesh;
			mesh = new Mesh(pPath, String<32>("MESH"));
		}

		mesh->load();
	}

	void exportBin(s32 binIndex)
	{
		if (binIndex > meshes.size() - 1)
			return;

		auto mesh = meshes[binIndex];

		if (mesh == nullptr)
			return;

		mesh->save(mesh->getPath());
	}

	void setMeshPath(s32 binIndex, String128& path)
	{
		if (binIndex > meshes.size() - 1)
			return;

		auto mesh = meshes[binIndex];

		if (mesh == nullptr)
			return;

		mesh->setPath(path);
	}

	void exportBinV10(s32 binIndex)
	{
		if (binIndex > meshes.size() - 1)
			return;

		auto mesh = meshes[binIndex];

		if (mesh == nullptr)
			return;

		mesh->saveBinV10(mesh->getPath());
	}

	void exportBinV11(s32 binIndex)
	{
		if (binIndex > meshes.size() - 1)
			return;

		auto mesh = meshes[binIndex];

		if (mesh == nullptr)
			return;

		mesh->saveBinV11(mesh->getPath());
	}

	void binFromObj(s32 objIndex, s32 binIndex);

	void setTriListMaterialID(s32 binIndex, s32 triListIndex, MaterialID matID)
	{
		if (binIndex > meshes.size() - 1)
			return;

		auto mesh = meshes[binIndex];

		if (triListIndex > mesh->triangleLists.size() - 1)
			return;

		auto& triList = mesh->triangleLists[triListIndex];

		triList.material.matID = matID;
	}

	void unloadObj(s32 objIndex)
	{
		if (objIndex > objMeshDatas.size() - 1)
			return;

		objMeshDatas.erase(objMeshDatas.begin() + objIndex);
	}

	s32 newMesh()
	{
		meshes.push_back(new Mesh());
		return meshes.size() - 1;
	}

	s32 newObj()
	{
		objMeshDatas.push_back(new OBJMeshData());
		return objMeshDatas.size() - 1;
	}

	void printCurrentMeshList(StringGeneric& pStr)
	{
		for (int i = 0; i < meshes.size(); ++i)
		{
			std::string str; str = std::to_string(i);
			String<4> indStr(str.c_str());
			pStr.append(indStr);
			auto mesh = meshes[i];
			pStr.append(String<32>("    Mesh name is \"")).append(mesh->getName()).append('"').append('\n');
			pStr.append(String<32>("    Mesh path is \"")).append(mesh->getPath()).append('"').append('\n');
			std::string size = std::to_string(mesh->getSize());
			String<32> sizeStr; sizeStr.setToChars(size.c_str());
			pStr.append(String<32>("    Mesh size is ")).append(sizeStr).append('B').append('\n');
		}
	}

	void printMeshInfo(s32 meshIndex, StringGeneric& pStr)
	{
		if (meshIndex > meshes.size() - 1)
			return;///TODO: Error string

		Mesh* mesh = meshes[meshIndex];

		if (mesh == nullptr)
			return;

		pStr.clear('\0');

		pStr.append(String<32>("  Mesh name is \"")).append(mesh->getName()).append('"').append('\n');
		pStr.append(String<32>("  Mesh path is \"")).append(mesh->getPath()).append('"').append('\n');
		std::string size = std::to_string(mesh->getSize());
		String<32> sizeStr; sizeStr.setToChars(size.c_str());
		pStr.append(String<32>("  Mesh size is ")).append(sizeStr).append(' ').append('B').append('\n');
	}

	void nullAllMeshTextures(s32 meshIndex, GLTexture2D* nullTex)
	{
		if (meshIndex > meshes.size() - 1)
			return;

		Mesh* mesh = meshes[meshIndex];

		if (mesh == nullptr)
			return;

		for (auto itr = mesh->triangleLists.begin(); itr != mesh->triangleLists.end(); ++itr)
		{
			for (int i = 0; i < 4; ++i)
			{
				itr->material.albedo[i] = nullTex;
				itr->material.specular[i] = nullTex;
				itr->material.normal[i] = nullTex;
			}
			itr->material.alpha = nullTex;
		}
	}

	void setMeshAlbedoTexture(s32 meshIndex, s32 triListIndex, GLTexture2D* glTexture, s32 matTexIndex)
	{
		if (meshIndex > meshes.size() - 1)
			return;

		Mesh* mesh = meshes[meshIndex];

		if (mesh == nullptr)
			return;

		mesh->triangleLists[triListIndex].material.albedo[matTexIndex] = glTexture;
	}

	void setMeshSpecularTexture(s32 meshIndex, s32 triListIndex, GLTexture2D* glTexture, s32 matTexIndex)
	{
		if (meshIndex > meshes.size() - 1)
			return;

		Mesh* mesh = meshes[meshIndex];

		if (mesh == nullptr)
			return;

		mesh->triangleLists[triListIndex].material.specular[matTexIndex] = glTexture;
	}

	void setMeshNormalTexture(s32 meshIndex, s32 triListIndex, GLTexture2D* glTexture, s32 matTexIndex)
	{
		if (meshIndex > meshes.size() - 1)
			return;

		Mesh* mesh = meshes[meshIndex];

		if (mesh == nullptr)
			return;

		mesh->triangleLists[triListIndex].material.normal[matTexIndex] = glTexture;
	}

	void setMeshAlphaTexture(s32 meshIndex, s32 triListIndex, GLTexture2D* glTexture)
	{
		if (meshIndex > meshes.size() - 1)
			return;

		Mesh* mesh = meshes[meshIndex];

		if (mesh == nullptr)
			return;

		mesh->triangleLists[triListIndex].material.alpha = glTexture;
	}

	void addTriList(InterleavedVertexData& obj, Mesh& model)
	{
		//Mesh::TriangleList list;
		//list.material.matID = PNUU_T_S_N;
		//memcpy(list.data, obj.data, obj.size);
		//model.triangleLists.push_back(list);
	}

	void setMeshName(s32 meshIndex, String<32>& pName)
	{
		if (meshIndex > meshes.size() - 1)
			return;

		Mesh* mesh = meshes[meshIndex];

		if (mesh == nullptr)
			return;

		mesh->name.overwrite(pName);
	}

	void setObjName(s32 objIndex, String32& pName)
	{
		if (objIndex > objMeshDatas.size() - 1)
			return;

		OBJMeshData* obj = objMeshDatas[objIndex];

		if (obj == nullptr)
			return;

		obj->name.overwrite(pName);
	}

	void setMeshDiskPath(s32 meshIndex, String<128>& pPath)
	{
		if (meshIndex > meshes.size() - 1)
			return;

		Mesh* mesh = meshes[meshIndex];

		if (mesh == nullptr)
			return;

		mesh->diskPath.overwrite(pPath);
	}

	void addMeshToTriLists(s32 takeMeshIndex, s32 triIndex, s32 addMeshIndex)
	{
		if (takeMeshIndex > meshes.size() - 1)
			return;

		if (addMeshIndex > meshes.size() - 1)
			return;

		Mesh* takeMesh = meshes[takeMeshIndex];
		Mesh* addMesh = meshes[addMeshIndex];

		if (takeMesh == nullptr)
			return;

		if (addMesh == nullptr)
			return;

		Mesh::TriangleList tl;
		tl.copyConstruct(takeMesh->triangleLists[triIndex]);

		addMesh->triangleLists.push_back(tl);
	}

	s32 objToBin(String<128>& objPath, String<128>& binPath, String<32>& name)
	{
		auto objInd = newObj();
		auto binInd = newMesh();
		loadOBJ(objPath, objInd);
		objMeshDatas[objInd]->name.overwrite(name);
		binFromObj(objInd, binInd);
		setMeshDiskPath(binInd, binPath);
		unloadObj(objInd);
		return binInd;
	}

	void scaleTexCoords(s32 meshIndex, float scale)
	{
		auto mesh = validateMesh(meshIndex);
		if (!mesh)
			return;

		for (auto itr = mesh->triangleLists.begin(); itr != mesh->triangleLists.end(); ++itr)
		{
			auto d = itr->data;
			for (int i = 0; i < itr->getDataSizeInFloats(); i += 8)
			{
				d[i + 3] *= scale;
				d[i + 4] *= scale;
			}
		}
	}

	void scaleAlphaTexCoords(s32 meshIndex, float scale)
	{
		auto mesh = validateMesh(meshIndex);
		if (!mesh)
			return;

		for (auto itr = mesh->triangleLists.begin(); itr != mesh->triangleLists.end(); ++itr)
		{
			itr->material.alphaScale = scale;
		}
	}

	Mesh* validateMesh(s32 meshIndex)
	{
		if (meshIndex > meshes.size() - 1)
			return nullptr;

		return meshes[meshIndex];
	}

	void clearStorage()
	{
		for (int i = 0; i < meshes.size(); ++i)
		{
			auto m = meshes[i];
			delete m;
		}
		meshes.clear();
		for (int i = 0; i < objMeshDatas.size(); ++i)
		{
			delete objMeshDatas[i];
		}
		objMeshDatas.clear();
	}

	std::vector<Mesh*> meshes;
	std::vector<OBJMeshData*> objMeshDatas;
};