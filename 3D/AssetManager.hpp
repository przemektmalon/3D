#pragma once
#include "Include.hpp"
#include "Font.hpp"
#include "Mesh.hpp"
#include "ImageData.hpp"
#include <unordered_map>

#include <Windows.h>

#include "GPUMeshManager.hpp"
#include "GPUTextureManager.hpp"

class AssetManager
{
public:
	AssetManager();
	~AssetManager();

	Asset* prepareAsset(Asset::Type pType, String<128>& pPath, String<32>& pName);

	Texture2D* prepareTexture(String<128>&& pPath, String<32>&& pName)
	{
		return (Texture2D*)prepareAsset(Asset::Type::Texture2D, pPath, pName);
	}

	Font* prepareFont(String<128>&& pPath, String<32>&& pName)
	{
		return (Font*)prepareAsset(Asset::Type::Font, pPath, pName);
	}

	Mesh* prepareMesh(String<128>&& pPath, String<32>&& pName)
	{
		return (Mesh*)prepareAsset(Asset::Type::Mesh, pPath, pName);
	}

	Mesh* prepareMesh(String<128>&& pPath, String<32>& pName)
	{
		return (Mesh*)prepareAsset(Asset::Type::Mesh, pPath, pName);
	}

	Model* prepareModel(String128& pPath, String32& pName)
	{
		return (Model*)prepareAsset(Asset::Type::Model, pPath, pName);
	}

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

	Texture2D* get2DTex(String32& pName)
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
	Texture2D* get2DTex(String<32>&& pName)
	{
		return get2DTex(pName);
	}

	GLTexture2D* get2DTexGL(String32& pName)
	{
		auto find = texture2DList.find(pName);
		if (find == texture2DList.end())
		{
			auto findNull = texture2DList.find(String32("null"));
			if (findNull == texture2DList.end())
				return nullptr;
			return findNull->second.glData;
		}
		return find->second.glData;
	}
	GLTexture2D* get2DTexGL(String32&& pName)
	{
		return get2DTexGL(pName);
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

	Model* getModel(String32& pName)
	{
		auto find = modelList.find(pName);
		if (find == modelList.end())
			return nullptr;
		return &find->second;
	}

	void removeTexture(String32& pName)
	{
		//auto find = texture2DList.find(pName);
		//if (find == texture2DList.end())
		//	return;
		texture2DList.erase(pName);
	}

	void removeMesh(String32& pName)
	{
		meshList.erase(pName);
	}

	void removeFont(String32& pName)
	{
		fontList.erase(pName);
	}

	GPUMeshManager meshManager;
	GPUTextureManager textureManager;

	std::map<String32, Font>& getFontList()
	{
		return fontList;
	}

	std::map<String32, Texture2D>& getTextureList()
	{
		return texture2DList;
	}

	void pushTexturesToGPU();

private:

	class AssetLoader
	{
	public:
		AssetLoader() {}
		~AssetLoader() {}

		void loadAssets(String128& assetListFilePath);
	};



	std::map<String32, Font> fontList;
	std::map<String32, Mesh> meshList;
	std::map<String32, Model> modelList;
	std::map<String32, Texture2D> texture2DList;
	std::map<String32, GLTextureCube> textureCubeList;

public:

	AssetLoader loader;
};
