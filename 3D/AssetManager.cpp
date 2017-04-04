#include "AssetManager.h"

AssetManager::AssetManager()
{
}

AssetManager::~AssetManager()
{
}

Asset* AssetManager::prepareAsset(Asset::Type pType, String<128>& pPath, String<32>& pName)
{
	switch (pType)
	{
	case Asset::Font: {
		auto ins = fontList.insert(std::make_pair(pName, Font(pPath, pName)));
		return &ins.first->second; }
	case Asset::Mesh: {
		//auto ins2 = meshList.insert(std::make_pair(pName, Mesh(pPath, pName)));
		auto ins2 = meshList.try_emplace(pName, pPath, pName);
		return &ins2.first->second; }
	case Asset::Texture2D: {
		auto ins3 = texture2DList.insert(std::make_pair(pName, GLTexture2D(pPath, pName, false)));
		return &ins3.first->second; }
	case Asset::Texture2DMip: {
		auto ins3 = texture2DList.insert(std::make_pair(pName, GLTexture2D(pPath, pName, true)));
		return &ins3.first->second; }
	}
}