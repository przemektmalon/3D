#pragma once
#include "Strings.hpp"

class Asset
{
public:
	Asset();
	Asset(String128& pPath, String32& pName);
	~Asset();

	enum Type { Null, Font, Texture2D, TextureCube, Mesh, Model, NTexture2D };

	void prepare(String128& pPath, String32& pName);

	virtual void load() = 0;

	u32 getSize() { return size; }
	bool doesExist() { return existsOnDisk; }
	String128& getPath() { return diskPath; }
	String32& getName() { return name; }

protected:
	void setPath(String128& pPath)
	{
		diskPath.overwrite(pPath);
	}

	String32 name;
	String128 diskPath;
	bool existsOnDisk;
	u32 size;
	Type type;
	u32 status;
};