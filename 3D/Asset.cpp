#include "Asset.h"

Asset::Asset() {}
Asset::Asset(String<128>& pPath, String<32>& pName) {
	prepare(pPath, pName);
}

Asset::~Asset() {}

void Asset::prepare(String<128>& pPath, String<32>& pName)
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