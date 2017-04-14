#pragma once
#include "Include.h"
#include "Font.h"
#include "Mesh.h"
#include <unordered_map>

//#include "Shlwapi.h"//TODO: MULTI-PLATFORMIZE
#include <Windows.h>

#include "GPUMeshManager.h"

class AssetManager
{
public:
	AssetManager();
	~AssetManager();

	Asset* prepareAsset(Asset::Type pType, String<128>& pPath, String<32>& pName);

	Asset* prepareAsset(Asset::Type pType, String<128>&& pPath, String<32>&& pName)
	{
		return prepareAsset(pType, pPath, pName);
	}

	Asset* prepareAsset(Asset::Type pType, String<128>& pPath, String<32>&& pName)
	{
		return prepareAsset(pType, pPath, pName);
	}

	Asset* prepareAsset(Asset::Type pType, String<128>&& pPath, String<32>& pName)
	{
		return prepareAsset(pType, pPath, pName);
	}

	void initMeshBatch()
	{
		meshManager.newBatch();
	}

	Font* getFont(String32& pName)
	{
		auto find = fontList.find(pName);
		if (find == fontList.end())
			return nullptr;//TODO: Error

		return &find->second;
	}
	Font* getFont(String32&& pName)
	{
		return getFont(pName);
	}

	GLTexture2D* get2DTex(String32& pName)
	{
		auto find = texture2DList.find(pName);
		if (find == texture2DList.end())
		{
			auto findNull = texture2DList.find(String32("null"));
			if (findNull == texture2DList.end())
				return nullptr;
			return &findNull->second;
		}
		return &find->second;
	}
	GLTexture2D* get2DTex(String<32>&& pName)
	{
		return get2DTex(pName);
	}

	GLTextureCube getCubeTex(String32& pName);
	
	Mesh* getMesh(String32& pName)
	{
		auto find = meshList.find(pName);
		if (find == meshList.end())
			return nullptr;//TODO: Error

		return &find->second;
	}
	Mesh* getMesh(String32&& pName)
	{
		return getMesh(pName);
	}

	GPUMeshManager meshManager;

private:

	std::map<String32, Font> fontList;
	std::map<String32, Mesh> meshList;
	std::map<String32, GLTexture2D> texture2DList;

	//std::map<String32, GLTexture2DMip> texture2DMipList;
	std::map<String32, GLTextureCube> textureCubeList;


	u32 lastMeshID = 0xa0000000;
};
