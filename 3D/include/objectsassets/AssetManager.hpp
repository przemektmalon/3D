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

	Asset* prepareAsset(Asset::Type pType, std::string pPath, std::string pName);

	Texture2D* prepareTexture(std::string pPath, std::string pName)
	{
		return (Texture2D*)prepareAsset(Asset::Type::Texture2D, pPath, pName);
	}

	Font* prepareFont(std::string pPath, std::string pName)
	{
		return (Font*)prepareAsset(Asset::Type::Font, pPath, pName);
	}

	Model* prepareModel(std::string pPath, std::string pName)
	{
		return (Model*)prepareAsset(Asset::Type::Model, pPath, pName);
	}

	Font* getFont(std::string& pName)
	{
		auto find = fontList.find(pName);
		if (find == fontList.end())
			return nullptr;//TODO: Error

		return &find->second;
	}
	Font* getFont(std::string&& pName)
	{
		return getFont(pName);
	}

	Texture2D* get2DTex(std::string& pName)
	{
		auto find = texture2DList.find(pName);
		if (find == texture2DList.end())
		{
			auto findNull = texture2DList.find("null");
			if (findNull == texture2DList.end())
				return nullptr;
			return &findNull->second;
		}
		return &find->second;
	}

	Texture2D* get2DTex(std::string&& pName)
	{
		return get2DTex(pName);
	}

	GLTexture2D* get2DTexGL(std::string& pName)
	{
		auto find = texture2DList.find(pName);
		if (find == texture2DList.end())
		{
			auto findNull = texture2DList.find("null");
			if (findNull == texture2DList.end())
				return nullptr;
			return findNull->second.glData;
		}
		return find->second.glData;
	}

	GLTexture2D* get2DTexGL(std::string&& pName)
	{
		return get2DTexGL(pName);
	}

	GLTextureCube getCubeTex(std::string& pName);

	Model* getModel(std::string& pName)
	{
		auto find = modelList.find(pName);
		if (find == modelList.end())
			return nullptr;
		return (Model*)&find->second;
	}

	void removeTexture(std::string& pName)
	{
		//auto find = texture2DList.find(pName);
		//if (find == texture2DList.end())
		//	return;
		texture2DList.erase(pName);
	}

	void removeFont(std::string& pName)
	{
		fontList.erase(pName);
	}

	std::map<std::string, Font>& getFontList()
	{
		return fontList;
	}

	std::map<std::string, Texture2D>& getTextureList()
	{
		return texture2DList;
	}

	void loadAssets(std::string assetListFilePath);

	MaterialMeta& getMaterial(std::string pName)
	{
		return materialLibrary.getMaterial(pName);
	}

	GPUModelManager modelManager;
	MaterialLibrary materialLibrary;

private:

	std::map<std::string, Font> fontList;
	std::map<std::string, Model> modelList;
	std::map<std::string, Texture2D> texture2DList;
	std::map<std::string, GLTextureCube> textureCubeList;
};
