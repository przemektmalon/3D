#pragma once
#include <string>

class GLTexture;

struct TextureMeta
{
	std::string name;
	GLTexture* glTex;

	TextureMeta() : glTex(nullptr) {}
	TextureMeta(std::string pName, GLTexture* pGLTex) : name(pName), glTex(pGLTex) {}

	TextureMeta& operator=(const TextureMeta& rhs)
	{
		name = rhs.name;
		glTex = rhs.glTex;

		return *this;
	}
};

struct MaterialMeta
{
	bool splatted;
	TextureMeta albedo, normal, specularMetallic, roughness, ao, height;

	MaterialMeta() : splatted(false) {}

	MaterialMeta& operator=(MaterialMeta& rhs)
	{
		albedo = rhs.albedo;
		normal = rhs.normal;
		specularMetallic = rhs.specularMetallic;
		roughness = rhs.roughness;
		ao = rhs.ao;
		height = rhs.height;
		
		return *this;
	}
};