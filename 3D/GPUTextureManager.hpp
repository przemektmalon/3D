#pragma once
#include "Texture.hpp"



class GPUTextureManager
{
public:
	GPUTextureManager() {}
	~GPUTextureManager() {}

	GPUTextureMeta pushToGPU(Texture2D* pTexture)
	{
		if (pTexture->data.width != pTexture->data.height)
			return GPUTextureMeta();

		auto find = texArrays.find(pTexture->data.width);

		if (find == texArrays.end())
		{
			find = texArrays.insert(std::make_pair(pTexture->data.width, GLTextureArray2D())).first;
			find->second.create(10, GL_RGBA8, pTexture->data.width, pTexture->data.height);
		}

		auto texIndex = find->second.pushTexture(pTexture);

		find->second.generateMips();

		return GPUTextureMeta(&find->second, texIndex);
	}

	GPUTextureMeta getTextureMeta(String32& pName)
	{
		for (auto itr = texArrays.begin(); itr != texArrays.end(); ++itr)
		{
			u32 index = 0;
			for (auto tex = itr->second.textures.begin(); tex != itr->second.textures.end(); ++tex)
			{
				if (*tex == nullptr)
					continue;
				if ((*tex)->getName() == pName)
				{
					return GPUTextureMeta(&(itr->second), index);
				}
			}
		}
		return GPUTextureMeta(nullptr, 0);
	}

	std::map<u32, GLTextureArray2D> texArrays; //Size , TextureArray

};