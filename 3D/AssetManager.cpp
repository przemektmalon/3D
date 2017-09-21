#include "AssetManager.hpp"

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
	case Asset::Model: {
		//auto ins2 = meshList.insert(std::make_pair(pName, Mesh(pPath, pName)));
		auto ins3 = modelList.try_emplace(pName, pPath, pName);
		return &ins3.first->second; }
	case Asset::Texture2D: {
		auto ins4 = texture2DList.insert(std::make_pair(pName, Texture2D(pPath, pName)));
		return &ins4.first->second; }
	}
}

void AssetManager::AssetLoader::loadAssets(String128 & assetListFilePath)
{
	std::ifstream file;
	file.open(assetListFilePath.getString());

	std::string section;

	while (!file.eof())
	{
		std::string line;
		while (std::getline(file, line))
		{
			if (line.length() < 3)
				continue;

			
			//std::istringstream istempline(line);
			std::string key;
			std::string value;

			auto getUntil = [](std::string& src, char delim)-> std::string {
				auto ret = src.substr(0, src.find(delim) == -1 ? 0 : src.find(delim));
				src.erase(0, src.find(delim)+1);
				return ret;
			};


			if (line == "[Texture]")
			{
				std::string name, path, mip;
				std::getline(file, key, '=');
				std::getline(file, value);
				path = value;
				std::getline(file, key, '=');
				std::getline(file, value);
				name = value;
				std::getline(file, key, '=');
				std::getline(file, value);
				mip = value;

				auto find = Engine::assets.texture2DList.find(String32(name.c_str()));
				if (find != Engine::assets.texture2DList.end())
				{
					std::cout << "Texture \"" << name << "\" already loaded" << std::endl;
					continue;
				}

				if (mip == "true")
				{
					auto tex = Engine::assets.prepareTexture(String128(path.c_str()), String128(name.c_str()));
					if (tex->doesExist())
					{
						tex->load();
					}
					else
					{
						std::cout << "Texture at " << path << " does not exist" << std::endl;
					}
					//tex->gpuMeta = Engine::assets.textureManager.pushToGPU(tex);
					tex->makeGLAsset();
					tex->glData->loadToGPU();
				}
				else
				{
					auto tex = Engine::assets.prepareTexture(String128(path.c_str()), String128(name.c_str()));
					if (tex->doesExist())
					{
						tex->load();
					}
					else
					{
						std::cout << "Texture at \"" << path << "\" does not exist" << std::endl;
					}
					//tex->gpuMeta = Engine::assets.textureManager.pushToGPU(tex);
					tex->makeGLAsset();
					tex->glData->loadToGPU();
				}
				
			}
			else if (line == "[Mesh]")
			{
				std::string line;
				std::getline(file, line);
				std::string name, path, ext, version;

				auto place = [&](std::string key, std::string value) -> bool {
					if (key == "path")
					{
						path = value;
						return true;
					}
					else if (key == "name")
					{
						name = value;
						return true;
					}
					else if (key == "ext")
					{
						ext = value;
						return true;
					}
					else if (key == "version")
					{
						version = value;
						return true;
					}
					return false;
				};

				key = getUntil(line, '=');

				while(key != "")
				{
					value = line;
					place(key, value);
					std::getline(file, line);
					key = getUntil(line, '=');
				}

				auto find = Engine::assets.meshList.find(String32(name.c_str()));
				if (find != Engine::assets.meshList.end())
				{
					std::cout << "Mesh \"" << name << "\" already loaded" << std::endl;
					continue;
				}

				auto mesh = Engine::assets.prepareMesh(String128(path.c_str()), String128(name.c_str()));
				if (mesh->doesExist())
				{
					if(version == "12")
					{
						mesh->loadBinV12();
					}
					else
					{
						mesh->load();
					}
				}
				else
				{
					std::cout << "Mesh at \"" << path << "\" does not exist" << std::endl;
				}
			}
			else if (line == "[Font]")
			{
				std::string name, path, ext;
				std::getline(file, key, '=');
				std::getline(file, value);
				path = value;
				std::getline(file, key, '=');
				std::getline(file, value);
				name = value;

				auto find = Engine::assets.fontList.find(String32(name.c_str()));
				if (find != Engine::assets.fontList.end())
				{
					std::cout << "Font \"" << name << "\" already loaded" << std::endl;
					continue;
				}

				auto font = Engine::assets.prepareFont(String128(path.c_str()), String128(name.c_str()));
				if (font->doesExist())
				{
					font->load();
				}
				else
				{
					std::cout << "Font at \"" << path << "\" does not exist" << std::endl;
				}
			}
		}
	}
	file.close();
}

void AssetManager::pushTexturesToGPU()
{
	for (auto tex : texture2DList)
	{
		std::cout << const_cast<String32&>(tex.first).getString() << std::endl;
	}
}