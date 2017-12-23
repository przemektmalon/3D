#pragma once
#include <string>
#include "Types.hpp"

class Asset
{
public:
	Asset();
	Asset(std::string& pPath, std::string& pName);
	~Asset();

	enum Type { Null, Font, Texture2D, TextureCube, Mesh, Model, NTexture2D };

	void prepare(std::string& pPath, std::string& pName);

	virtual void load() = 0;

	u32 getSize() { return size; }
	bool doesExist() { return existsOnDisk; }
	std::string getPath() { return diskPath; }
	std::string getName() { return name; }

protected:
	void setPath(std::string& pPath)
	{
		diskPath = pPath;
	}

	std::string name;
	std::string diskPath;
	bool existsOnDisk;
	u32 size;
	Type type;
	u32 status;
};