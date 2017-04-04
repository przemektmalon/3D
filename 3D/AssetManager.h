#pragma once
#include "Include.h"
#include "Font.h"
#include "Mesh.h"
#include <unordered_map>

//#include "Shlwapi.h"//TODO: MULTI-PLATFORMIZE
#include <Windows.h>


class AssetManager
{
public:
	AssetManager();
	~AssetManager();

	Asset& prepareAsset(Asset::Type pType, String128& pPath, String32& pName)
	{
		switch (pType)
		{
		case Asset::Font: {
			auto ins = fontList.insert(std::make_pair(pName, Font(pPath, pName)));
			return ins.first->second; }
		case Asset::Mesh: {
			auto ins2 = meshList.insert(std::make_pair(pName, Mesh(pPath, pName)));
			return ins2.first->second; }
		}
	}

	Asset& prepareAsset(Asset::Type pType, String128&& pPath, String32&& pName)
	{
		return prepareAsset(pType, pPath, pName);
	}

	Asset& prepareAsset(Asset::Type pType, String128& pPath, String32&& pName)
	{
		return prepareAsset(pType, pPath, pName);
	}

	Asset& prepareAsset(Asset::Type pType, String128&& pPath, String32& pName)
	{
		return prepareAsset(pType, pPath, pName);
	}

	void init()
	{
		meshManager.newBatch();
	}

	Font& getFont(String32& pName)
	{
		return fontList.at(pName);
	}
	Font& getFont(String32&& pName)
	{
		return fontList.at(pName);
	}

	GLTexture2D* get2DTex(std::string pName);
	GLTexture2DMip get2DMipTex(std::string pName);
	GLTextureCube getCubeTex(std::string pName);

	Mesh& getMesh(String32& pName)
	{
		return meshList.at(pName);
	}
	Mesh& getMesh(String32&& pName)
	{
		return meshList.at(pName);
	}

	GPUMeshManager meshManager;

private:



	//std::map<std::string, u32> meshNames;
	//std::map<u32, MeshCPUMeta> meshList;

	std::map<String32, Font> fontList;
	std::map<String32, Mesh> meshList;
	std::map<String32, GLTexture2D> texture2DList;
	std::map<String32, GLTexture2DMip> texture2DMipList;
	std::map<String32, GLTextureCube> textureCubeList;

	u32 lastMeshID = 0xa0000000;
};
