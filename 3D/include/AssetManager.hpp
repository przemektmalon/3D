#pragma once
#include "Include.hpp"
#include "ImageData.hpp"
#include "Font.hpp"
#include <map>
#include "GPUModelManager.hpp"
#include "MaterialLibrary.hpp"
#include "Asset.hpp"
#include "Model.hpp"

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

	GLTexture2D* get2DTexGL(std::string pName)
	{
		return get2DTexGL(String32(pName.c_str()));
	}

	GLTextureCube getCubeTex(String32& pName);

	Model* getModel(String32& pName)
	{
		auto find = modelList.find(pName);
		if (find == modelList.end())
			return nullptr;
		return (Model*)&find->second;
	}

	void removeTexture(String32& pName)
	{
		//auto find = texture2DList.find(pName);
		//if (find == texture2DList.end())
		//	return;
		texture2DList.erase(pName);
	}

	void removeFont(String32& pName)
	{
		fontList.erase(pName);
	}

	std::map<String32, Font>& getFontList()
	{
		return fontList;
	}

	std::map<String32, Texture2D>& getTextureList()
	{
		return texture2DList;
	}

	void loadAssets(String128& assetListFilePath);

	MaterialMeta& getMaterial(std::string pName)
	{
		return materialLibrary.getMaterial(pName);
	}

	GPUModelManager modelManager;
	MaterialLibrary materialLibrary;

private:

	std::map<String32, Font> fontList;
	std::map<String32, Model> modelList;
	std::map<String32, Texture2D> texture2DList;
	std::map<String32, GLTextureCube> textureCubeList;
};
