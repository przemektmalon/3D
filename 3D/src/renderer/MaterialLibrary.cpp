#include "MaterialLibrary.hpp"
#include "Engine.hpp"
#include "AssetManager.hpp"

void MaterialLibrary::addMaterial(std::string name, std::string albedo, std::string normal, std::string specMetal, std::string roughness, std::string ao, std::string height)
{
	auto find = materials.find(name);
	if (find != materials.end())
		return; /// TODO: log ? material already exists

	materials[name].albedo = TextureMeta(albedo, Engine::assets.get2DTexGL(albedo));
	materials[name].normal = TextureMeta(normal, Engine::assets.get2DTexGL(normal));
	materials[name].specularMetallic = TextureMeta(specMetal, Engine::assets.get2DTexGL(specMetal));
	materials[name].roughness = TextureMeta(roughness, Engine::assets.get2DTexGL(roughness));
	materials[name].ao = TextureMeta(ao, Engine::assets.get2DTexGL(ao));
	materials[name].height = TextureMeta(height, Engine::assets.get2DTexGL(height));
}