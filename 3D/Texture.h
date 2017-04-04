#pragma once
#include "Include.h"
#include "SOIL.h"
#include <Windows.h>

class GLTexture
{
public:
	GLTexture() {}
	~GLTexture() {}

	void release()
	{
		glDeleteTextures(1, &GLID);
	}

	virtual GLuint getGLID() { return GLID; }
	virtual GLuint64 getHandle(GLuint pSampler) { return glGetTextureSamplerHandleARB(GLID, pSampler); }

	virtual void bind(GLint pTextureUnit = 0) = 0;

	void setGLID(GLuint pG) { GLID = pG; }

protected:

	GLint type;
	GLint format;
	GLint internalFormat;
	GLint pixelAlignment;
	GLuint GLID;
	s32 width;
	s32 height;
};

class GLTextureCube : public GLTexture
{
public:
	GLTextureCube() {}
	~GLTextureCube() {}

	void createFromFiles(const char* pFilePaths[6])
	{
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &GLID);
		unsigned char* image;
		int tHeight;
		for (int i = 0; i < 6; ++i)
		{
			image = SOIL_load_image(pFilePaths[i], &width, &tHeight, 0, SOIL_LOAD_RGB);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, width, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			SOIL_free_image_data(image);
		}
	}

	void bind(GLint pTextureUnit = 0)
	{
		glBindTextureUnit(pTextureUnit, GLID);
	}

	void bindImage(GLint pImageUnit, GLenum pAccess)
	{
		glBindImageTexture(pImageUnit, GLID, 0, GL_FALSE, 0, pAccess, internalFormat);
	}

	void saveToFiles(const char* pFilePaths[6])
	{

	}
};

class GLTexture2D : public GLTexture
{
public:
	GLTexture2D() {}
	~GLTexture2D() { }

	virtual void createFromFile(std::string pFilePath)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &GLID);
		glBindTexture(GL_TEXTURE_2D, GLID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		unsigned char* image = SOIL_load_image(pFilePath.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	} //TODO: Implement load from file
	virtual void createFromStream(GLint pInternalFormat, s32 pWidth, s32 pHeight, GLint pFormat, GLenum pType, const void* pPixels, GLint pTextureUnit = 0, GLenum pPixelAlignment = 4)
	{
		internalFormat = pInternalFormat;
		width = pWidth; height = pHeight;
		format = pFormat;
		type = pType;
		pixelAlignment = pPixelAlignment;
		glPixelStorei(GL_PACK_ALIGNMENT, pixelAlignment);
		glPixelStorei(GL_UNPACK_ALIGNMENT, pixelAlignment);
		glActiveTexture(GL_TEXTURE0);
		glCreateTextures(GL_TEXTURE_2D, 1, &GLID);
		glBindTextureUnit(0, GLID);

		//glGenTextures(1, &GLID);
		glBindTexture(GL_TEXTURE_2D, GLID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, pInternalFormat, pWidth, pHeight, 0, pFormat, pType, pPixels);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void bind(GLint pTextureUnit = 0)
	{
		glBindTextureUnit(pTextureUnit, GLID);
	}

	void bindImage(GLint pImageUnit, GLenum pAccess)
	{
		glBindImageTexture(pImageUnit, GLID, 0, GL_FALSE, 0, pAccess, internalFormat);
	}

	s32 getWidth() { return width; }
	s32 getHeight() { return height; }

	void saveToFile(std::string fileName = "___AUTO_GENERATE_NAME___", bool flipY = false)
	{
		const int w = width, h = height;
		u32 sourcePixelSize;
		switch (internalFormat)
		{
		case(GL_RED):
			sourcePixelSize = 1;
			break;
		case(GL_RGB):
			sourcePixelSize = 3;
			break;
		case(GL_RGB16F):
			sourcePixelSize = 6;
			break;
		case(GL_RGB32F):
			sourcePixelSize = 12;
			break;
		case(GL_RGBA):
			sourcePixelSize = 4;
			break;
		case(GL_RGBA16F):
			sourcePixelSize = 8;
			break;
		case(GL_RGBA32F):
			sourcePixelSize = 16;
			break;
		default:
			sourcePixelSize = 4;
		}
		const u32 targetPixelSize = 3;

		glPixelStorei(GL_UNPACK_ALIGNMENT, pixelAlignment);

		u32 targetImageSize = h * w * targetPixelSize;
		u32 sourceImageSize = h * w * sourcePixelSize;

		u32 targetRowSize = w * targetPixelSize;
		u32 sourceRowSize = w * sourcePixelSize;

		u8* screenshot = new u8[sourceImageSize];
		u8* flipped = new u8[targetImageSize];

		std::string filepath = "res/screenshot/" + fileName + ".bmp";
		glBindTextureUnit(1, GLID);
		glActiveTexture(GL_TEXTURE1);
		glGetTexImage(GL_TEXTURE_2D, 0, internalFormat, type, screenshot);

		float flipSign = flipY == true ? -1.f : 1.f;
		float flip = flipY == true ? sourceImageSize - sourceRowSize : 0;

		for (int y = 0; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				u32 sourceOffset = flip + (flipSign * (y * sourceRowSize) + (x * sourcePixelSize));
				u32 targetOffset = (y * targetRowSize) + (x * targetPixelSize);
				flipped[targetOffset] = screenshot[sourceOffset];
				flipped[targetOffset + 1] = screenshot[sourceOffset + (sourcePixelSize == 1 ? 0 : 1)];
				flipped[targetOffset + 2] = screenshot[sourceOffset + (sourcePixelSize == 1 ? 0 : 2)];
			}
		}



		FILE *f;
		int filesize = 54 + targetImageSize;

		unsigned char bmpfileheader[14] = { 'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0 };
		unsigned char bmpinfoheader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0 };
		unsigned char bmppad[3] = { 0,0,0 };

		bmpfileheader[2] = (unsigned char)(filesize);
		bmpfileheader[3] = (unsigned char)(filesize >> 8);
		bmpfileheader[4] = (unsigned char)(filesize >> 16);
		bmpfileheader[5] = (unsigned char)(filesize >> 24);

		//int w = w; int h = h;

		bmpinfoheader[4] = (unsigned char)(w);
		bmpinfoheader[5] = (unsigned char)(w >> 8);
		bmpinfoheader[6] = (unsigned char)(w >> 16);
		bmpinfoheader[7] = (unsigned char)(w >> 24);
		bmpinfoheader[8] = (unsigned char)(h);
		bmpinfoheader[9] = (unsigned char)(h >> 8);
		bmpinfoheader[10] = (unsigned char)(h >> 16);
		bmpinfoheader[11] = (unsigned char)(h >> 24);

		auto er = fopen_s(&f, filepath.c_str(), "wb");
		fwrite(bmpfileheader, 1, 14, f);
		fwrite(bmpinfoheader, 1, 40, f);

		fwrite(flipped, 3, w * h, f);

		fclose(f);

		delete[] screenshot;
		delete[] flipped;
	}

private:
};

class GLTexture2DMip : public GLTexture2D
{
public:
	GLTexture2DMip() {}
	~GLTexture2DMip() {}

	virtual void createFromFile(std::string pFilePath)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &GLID);
		glBindTexture(GL_TEXTURE_2D, GLID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		unsigned char* image = SOIL_load_image(pFilePath.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		auto c = SOIL_last_result();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	void createFromStream(GLint pInternalFormat, s32 pWidth, s32 pHeight, GLint pFormat, GLenum pType, const void* pPixels, s32 mipLevels, s8 pTextureUnit = 0)
	{
		glActiveTexture(GL_TEXTURE0 + pTextureUnit);
		glGenTextures(1, &GLID);
		glBindTexture(GL_TEXTURE_2D, GLID);
		glTexImage2D(GL_TEXTURE_2D, 0, pInternalFormat, pWidth, pHeight, 0, pFormat, pType, pPixels);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
};

class GLRenderbuffer
{
public:
	GLRenderbuffer() {}
	~GLRenderbuffer() {}

	virtual void create(GLint pInternalFormat, s32 pWidth, s32 pHeight)
	{
		internalFormat = pInternalFormat;
		glGenRenderbuffers(1, &GLID);
		glBindRenderbuffer(GL_RENDERBUFFER, GLID);
		glRenderbufferStorage(GL_RENDERBUFFER, pInternalFormat, pWidth, pHeight);
	}

	void bind()
	{
		glBindRenderbuffer(GL_RENDERBUFFER, GLID);
	}

	GLuint getGLID() { return GLID; }

protected:

	GLint internalFormat;
	GLuint GLID;
};