#pragma once
#include <Windows.h>
#include <errno.h>

class Asset
{
public:
	Asset() {}
	Asset(String128& pPath, String32& pName) { prepare(pPath, pName); }
	~Asset() {}

	enum Type { Font, Texture2D, Texture2DMip, TextureCube, Mesh };

	void prepare(String128& pPath, String32& pName)
	{
		diskPath.overwrite(pPath);
		name.overwrite(pName);
		if (existsOnDisk)
		{
			struct _stat buf;
			errno_t err;

			int result = _stat(pPath.getString(), &buf);

			if (result != 0) {
				existsOnDisk = false;
				size = 0;
			}
			else {
				existsOnDisk = true;
				size = buf.st_size;
			}
		}
	}

	virtual void load() {}

	u32 getSize() { return size; }
	bool doesExist() { return existsOnDisk; }
	const String128& getPath() { return diskPath; }
	const String32& getName() { return name; }

protected:
	String32 name;
	String128 diskPath;
	bool existsOnDisk;
	u32 size;
	Type type;
	u32 status;
};