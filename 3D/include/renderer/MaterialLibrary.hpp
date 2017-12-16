#pragma once
#include "Material.hpp"
#include <unordered_map>
#include <string>

class MaterialLibrary
{
public:
	MaterialLibrary() {}
	~MaterialLibrary() {}

	void addMaterial(std::string name, std::string albedo, std::string normal = std::string(""), std::string specMetal = std::string(""), std::string roughness = std::string(""), std::string ao = std::string(""), std::string height = std::string(""));

	MaterialMeta& getMaterial(std::string name)
	{
		auto& find = materials.find(name);
		if (find == materials.end())
			return materials.begin()->second; /// TODO: return a default null material

		return find->second;
	}

private:

	std::unordered_map<std::string, MaterialMeta> materials;

};