#include "Asset.hpp"

Asset::Asset() {}
Asset::Asset(String<128>& pPath, String<32>& pName) {
	prepare(pPath, pName);
	existsOnDisk = true;
}

Asset::~Asset() {}

void Asset::prepare(String<128>& pPath, String<32>& pName)
{
	diskPath.overwrite(pPath);
	name.overwrite(pName);
	existsOnDisk = true;
	if (existsOnDisk)
	{
		struct stat buf;
		errno_t err;

		//int result = _stat32(pPath.getString(), &buf);

		//if (result != 0) {
		//	existsOnDisk = false;
		//	size = 0;
		//}
		//else {
			
			//size = buf.st_size;
			size = 1;
		//}
	}
}