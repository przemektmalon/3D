#pragma once
#include "SOIL.h"
#include "Types.hpp"
#include <Windows.h>
#include "Asset.hpp"

class ImageData
{
public:
	enum Format { RED_INTEGER, RG, RGB, RGBA, ALPHA, DEPTH, RG16F, R32I, NUM_FORMATS };

	ImageData() : data(nullptr) {}
	~ImageData() {}

	void loadFromStream(char* pData, s32 pWidth, s32 pHeight, Format pFormat)
	{
		if (data)
			free();
		
		width = pWidth;
		height = pHeight;

		format = pFormat;

		if (!pData)
			return;

		data = (char*)malloc(getSize());

		memcpy(data, pData, getSize());
	}

	void loadFromFile(const char * pTextureLocation, Format pFormat)
	{
		format = pFormat;
		switch (format)
		{
		case RGB:
			data = (char*)SOIL_load_image(pTextureLocation, &width, &height, 0, SOIL_LOAD_RGB);
			break;
		case RGBA:
			data = (char*)SOIL_load_image(pTextureLocation, &width, &height, 0, SOIL_LOAD_RGBA);
			break;
		}
	}

	void free()
	{
		SOIL_free_image_data((unsigned char *)data);
		data = nullptr;
	}

	u32 getSize() { return  width * height * formatSizes[format]; }

	char* getData() { return data; }
	
	s32 width, height;

private:

	Format format;

	char* data;

	const static s32 formatSizes[];
};
