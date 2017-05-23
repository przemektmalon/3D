#pragma once

class StringGeneric;

void getDateTimeStr(StringGeneric& str);
void getTimeStr(StringGeneric& str, char delim);


/*#pragma once
#include "Engine.h"

float randRange(float min, float max)
{
	return min + float(Engine::rand()) / float(std::mt19937_64::max()/(max - min));
}

float randFloat()
{
	return float(Engine::rand()) / float(std::mt19937_64::max());
}

void saveTexToFile(GLuint texture, std::string targetPath)
{
	int w, h;
	int miplevel = 0;
	//glActiveTexture(GL_TEXTURE12);
	//glBindTexture(GL_TEXTURE_2D, texture);
	//glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
	//glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
	w = 160;
	h = 160;

	u32 sourcePixelSize = 1;
	const u32 targetPixelSize = 3;

	u32 targetImageSize = h * w * targetPixelSize;
	u32 sourceImageSize = h * w * sourcePixelSize;

	u32 targetRowSize = w * targetPixelSize;
	u32 sourceRowSize = w * sourcePixelSize;

	u8* screenshot = new u8[sourceImageSize];
	u8* flipped = new u8[targetImageSize];

	while (glGetError());

	std::string filepath = "screenshot/" + targetPath + ".bmp";
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, screenshot);

	auto err = glGetError();

	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			//u32 sourceOffset = sourceImageSize - sourceRowSize - (y * sourceRowSize) + (x * sourcePixelSize);
			//u32 targetOffset = y * targetRowSize + (x * targetPixelSize);
			u32 sourceOffset = (y * sourceRowSize) + (x * sourcePixelSize);
			u32 targetOffset = (y * targetRowSize) + (x * targetPixelSize);
			flipped[targetOffset] = screenshot[sourceOffset];
			flipped[targetOffset + 1] = screenshot[sourceOffset];
			flipped[targetOffset + 2] = screenshot[sourceOffset];
			//printf("%i", screenshot[sourceOffset]);
		}
	}

	SDL_Surface * surface = SDL_CreateRGBSurfaceFrom(flipped, w, h, 8 * targetPixelSize, w * targetPixelSize, 0, 0, 0, 0);
	SDL_SaveBMP(surface, filepath.c_str());
	SDL_FreeSurface(surface);
	delete[] screenshot;
	delete[] flipped;
}*/

#pragma once
#include "Engine.h"

//#undef max
//#undef min

#ifndef RANDS
#define RANDS

/*float randRange(float min, float max)
{
	//return min + float(Engine::rand()) / float(std::mt19937_64::max() / (max - min));
	return 0.1;
}

float randFloat()
{
	//return float(Engine::rand()) / float(std::mt19937_64::max());
	return 0.1;
}*/

#endif

//#define max(a,b)            (((a) > (b)) ? (a) : (b))
//#define min(a,b)            (((a) < (b)) ? (a) : (b))

/*void saveTexToFile(GLuint texture, int width, int height, GLenum texFormat, GLenum texType, std::string fileName, GLenum pixelAlignment = 0)
{
	const int w = width, h = height;
	texture = 0;
	u32 sourcePixelSize;
	switch (texFormat)
	{
	case(GL_RED) :
		sourcePixelSize = 1;
		break;
	case(GL_RGB) :
		sourcePixelSize = 3;
		break;
	case(GL_RGBA) :
		sourcePixelSize = 4;
		break;
	}
	const u32 targetPixelSize = 3;

	if (pixelAlignment == 0)
	{
		switch (texFormat)
		{
		case(GL_RED) :
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			break;
		case(GL_RGB) :
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			break;
		case(GL_RGBA) :
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			break;
		}
	}
	else
		glPixelStorei(GL_UNPACK_ALIGNMENT, pixelAlignment);

	u32 targetImageSize = h * w * targetPixelSize;
	u32 sourceImageSize = h * w * sourcePixelSize;

	u32 targetRowSize = w * targetPixelSize;
	u32 sourceRowSize = w * sourcePixelSize;

	u8* screenshot = new u8[sourceImageSize];
	u8* flipped = new u8[targetImageSize];

	std::string filepath = "screenshot/" + fileName + ".bmp";
	glBindTextureUnit(0, texture);
	glGetTexImage(GL_TEXTURE_2D, 0, texFormat, texType, screenshot);

	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			u32 sourceOffset = (y * sourceRowSize) + (x * sourcePixelSize);
			u32 targetOffset = (y * targetRowSize) + (x * targetPixelSize);
			flipped[targetOffset] = screenshot[sourceOffset];
			flipped[targetOffset + 1] = screenshot[sourceOffset + (sourcePixelSize == 1 ? 0 : 1)];
			flipped[targetOffset + 2] = screenshot[sourceOffset + (sourcePixelSize == 1 ? 0 : 2)];
			//flipped[targetOffset + 1] = screenshot[sourceOffset];
			//flipped[targetOffset + 2] = screenshot[sourceOffset];
		}
	}

	/*SDL_Surface * surface = SDL_CreateRGBSurfaceFrom(flipped, w, h, 8 * targetPixelSize, w * targetPixelSize, 0, 0, 0, 0);
	SDL_SaveBMP(surface, filepath.c_str());
	SDL_FreeSurface(surface);
	delete[] screenshot;
	delete[] flipped;
}*/