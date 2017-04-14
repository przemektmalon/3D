#pragma once
#include "Mesh.h"
#include "Strings.h"
#include "AssetManager.h"

class SeperatedVertexData
{
public:
	~SeperatedVertexData()
	{
		for (int i = 0; i < 12; ++i)
		{
			if (data[i] == nullptr)
				return;

			delete data[i];
			data[i] = nullptr;
		}
	}

	VertexFormat format;
	void* data[12];
};

class OBJMeshData
{
public:
	SeperatedVertexData vData;
	std::vector<int> indices;

	int getNumVerts() { return indices.size() / 3; }
	int getNumTris() { return getNumVerts() / 3; }

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

		mesh->saveBinV10(mesh->getPath());
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

	void binFromObj(s32 objIndex, s32 binIndex)
	{
		if (objIndex > objMeshDatas.size() - 1)
			return;

		if (binIndex > meshes.size() - 1)
			return;

		auto objMeshData = objMeshDatas[objIndex];
		auto mesh = meshes[binIndex];

		InterleavedVertexData ivd;
		ivd.fromOBJMeshData(objMeshData);

		//mesh->ivd = ivd;
		//mesh->intData.interlacedData = ivd.data;
		//mesh->intData.size = ivd.size;
		mesh->size = ivd.size;
		mesh->diskPath.overwrite(String<128>("PATH"));
		mesh->name.overwrite(String<32>("NAME"));

		Mesh::TriangleList list;
		list.material.matID = PNUU_T_S_N;
		list.data = new float[ivd.size];
		//list.first = Engine::assets.meshManager.solidBatches[mesh->renderMeta.batchIndex].firsts[mesh->renderMeta.batchID];
		list.first = 0;
		list.numVerts = ivd.size / ivd.format.size;
		memcpy(list.data, ivd.data, ivd.getDataSizeInBytes());
		mesh->triangleLists.push_back(list);
	}

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

		//if (texIndex > textures.size() + 1)
		//	return;

		mesh->triangleLists[triListIndex].material.albedo[matTexIndex] = glTexture;
	}

	void setMeshSpecularTexture(s32 meshIndex, s32 triListIndex, GLTexture2D* glTexture, s32 matTexIndex)
	{
		if (meshIndex > meshes.size() - 1)
			return;

		Mesh* mesh = meshes[meshIndex];

		if (mesh == nullptr)
			return;

		//if (texIndex > textures.size() + 1)
		//	return;

		mesh->triangleLists[triListIndex].material.specular[matTexIndex] = glTexture;
	}

	void setMeshNormalTexture(s32 meshIndex, s32 triListIndex, GLTexture2D* glTexture, s32 matTexIndex)
	{
		if (meshIndex > meshes.size() - 1)
			return;

		Mesh* mesh = meshes[meshIndex];

		if (mesh == nullptr)
			return;

		//if (texIndex > textures.size() + 1)
		//	return;

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
		Mesh::TriangleList list;
		list.material.matID = PNUU_T_S_N;
		memcpy(list.data, obj.data, obj.size);
		model.triangleLists.push_back(list);
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

	s32 objToBin(String<128>& objPath, String<128>& binPath)
	{
		auto objInd = newObj();
		auto binInd = newMesh();
		loadOBJ(objPath, objInd);
		binFromObj(objInd, binInd);
		setMeshDiskPath(binInd, binPath);
		unloadObj(objInd);
		return binInd;
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

	/*void clearTextures()
	{
		for (int i = 0; i < textures.size(); ++i)
		{
			delete textures[i];
		}
		textures.clear();
	}*/

	std::vector<Mesh*> meshes;
	std::vector<OBJMeshData*> objMeshDatas;
	//std::vector<GLTexture2D*> textures;
};