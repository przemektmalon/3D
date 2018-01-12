#pragma once
#include "SOIL.h"
#include "Asset.hpp"
#include "ImageData.hpp"
#include "GLIncludes.hpp"
#include "MathIncludes.hpp"
#include <vector>

#undef max
#undef min

static GLenum sizedToBaseFormat(GLenum sizedFormat)
{
	switch (sizedFormat)
	{
	case GL_R8:
		return GL_RED;
	case GL_RG8:
		return GL_RG;
	case GL_RGB8:
		return GL_RGB;
	case GL_RGBA8:
		return GL_RGBA;
	case GL_RG16F:
		return GL_RG;
	case GL_R32I:
		return GL_RED;
	case GL_DEPTH_COMPONENT32F_NV:
		return GL_DEPTH_COMPONENT;
	}
}

static GLsizei sizedFormatSize(GLenum sizedFormat)
{
	switch (sizedFormat)
	{
	case GL_R8:
		return 8;
	case GL_RG8:
		return 16;
	case GL_RGB8:
		return 24;
	case GL_RGBA8:
		return 32;
	case GL_RG16F:
		return 32;
	case GL_R32I:
		return 32;
	case GL_DEPTH_COMPONENT32F_NV:
		return 32;
	}
}

static ImageData::Format sizedFormatToID(GLenum sizedFormat)
{
	switch (sizedFormat)
	{
	case GL_R8:
		return ImageData::Format::ALPHA;
	case GL_RG8:
		return ImageData::Format::RG;
	case GL_RGB8:
		return ImageData::Format::RGB;
	case GL_RGBA8:
		return ImageData::Format::RGBA;
	case GL_RG16F:
		return ImageData::Format::RG16F;
	case GL_R32I:
		return ImageData::Format::R32I;
	case GL_DEPTH_COMPONENT32F_NV:
		return ImageData::Format::DEPTH;
	}
}

class GLTexture2D;

class GLTextureArray2D;

struct GPUTextureMeta
{
	GPUTextureMeta() : texArr(nullptr), index(-1) {}
	GPUTextureMeta(GLTextureArray2D* pTexArr, u32 pIndex) : texArr(pTexArr), index(pIndex) {}
	GLTextureArray2D* texArr;
	s32 index;

	friend bool operator<(GPUTextureMeta& lhs, GPUTextureMeta& rhs)
	{
		return ((lhs.texArr == rhs.texArr) && (lhs.index < rhs.index)) || ((lhs.texArr != rhs.texArr) && (lhs.texArr < rhs.texArr));

		/*
		if (lhs.texArr == rhs.texArr)
		return lhs.index < rhs.index;

		return lhs.texArr < rhs.texArr;
		*/
	}
};

class Texture2D : public Asset
{
public:
	ImageData data;
	GLTexture2D* glData;

	Texture2D() : glData(nullptr) {}
	Texture2D(std::string& pPath, std::string& pName) : Asset(pPath, pName), glData(nullptr) {}
	~Texture2D() {}

	void load()
	{
		data.loadFromFile(this->diskPath.c_str(), ImageData::Format::RGBA);
	}

	void makeGLAsset();

	char* getData() { 
		return data.getData(); 
	}
};

class GLTexture
{
protected:
	GLuint GLID;
	GLuint64 handle;
	s32 mipLevels;
	GLenum type;
	GLenum format;
	GLenum sizedFormat;

public:
	GLTexture() : format(0), sizedFormat(0), type(0), handle(0), GLID(0), mipLevels(0) {}

	void release() { glDeleteTextures(1, &GLID); }

	GLuint getGLID() { return GLID; }

	void bind(GLint pTextureUnit = 0)
	{
		glBindTextureUnit(pTextureUnit, GLID);
	}

	virtual GLuint64 getHandle(GLuint pSampler)
	{
		handle = glGetTextureSamplerHandleARB(GLID, pSampler);
		return handle;
	}
	virtual GLuint64 makeResident(GLuint pSampler)
	{
		if (handle == 0)
		{
			getHandle(pSampler);
		}
		glMakeTextureHandleResidentARB(handle);
		return handle;
	}
};

class GLTexture2D : public GLTexture
{
public:
	Texture2D* texture;
	ImageData* streamImageData;
	s32 width, height;

	GLTexture2D() : width(0), height(0), streamImageData(nullptr), texture(nullptr) {}
	~GLTexture2D() 
	{
		if (streamImageData)
			streamImageData->free();
	}

	

	void bindImage(GLint pImageUnit, GLenum pAccess, int pMipLevel = 0)
	{
		glBindImageTexture(pImageUnit, GLID, pMipLevel, GL_FALSE, 0, pAccess, sizedFormat);
	}

	void assignTexture(Texture2D* pTexture)
	{
		texture = pTexture;
		width = texture->data.width;
		height = texture->data.height;
	}

	void loadToGPU()
	{
		if (!texture)
			return; /// TODO: log error ?

		type = GL_UNSIGNED_BYTE;
		sizedFormat = GL_RGBA8;
		format = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &GLID);
		glBindTexture(GL_TEXTURE_2D, GLID);
		
		mipLevels = glm::floor(std::log2(glm::max(width, height))) + 1;

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipLevels);
		glTexImage2D(GL_TEXTURE_2D, 0, sizedFormat, width, height, 0, format, type, texture->data.getData());
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void createFromStream(GLint pSizedFormat, s32 pWidth, s32 pHeight, GLint pFormat, GLenum pType, const void* pPixels, GLint pTextureUnit = 0, GLint pPixelAlignment = 4, int pMipLevels = -1)
	{
		streamImageData = new ImageData();
		sizedFormat = pSizedFormat;
		format = pFormat;
		type = pType;
		width = pWidth;
		height = pHeight;

		ImageData::Format mFormat = sizedFormatToID(sizedFormat);

		streamImageData->loadFromStream((char*)pPixels, width, height, mFormat);

		glPixelStorei(GL_PACK_ALIGNMENT, pPixelAlignment);
		glPixelStorei(GL_UNPACK_ALIGNMENT, pPixelAlignment);

		glCreateTextures(GL_TEXTURE_2D, 1, &GLID);
		glBindTexture(GL_TEXTURE_2D, GLID);

		auto err = glGetError();

		int maxMipLevels = glm::floor(std::log2(glm::max(width, height))) + 1;

		if (pMipLevels == -1 || pMipLevels > maxMipLevels)
			mipLevels = maxMipLevels;
		else
			mipLevels = pMipLevels;

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipLevels);
		glTexStorage2D(GL_TEXTURE_2D, mipLevels, pSizedFormat, width, height);
		//glTexImage2D(GL_TEXTURE_2D, 0, sizedFormat, width, height, 0, format, type, 0);

		err = glGetError();

		if (streamImageData->getData())
		{
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, sizedToBaseFormat(sizedFormat), type, streamImageData->getData());
		}
		glGenerateMipmap(GL_TEXTURE_2D);

		err = glGetError();
	}

	s32 getWidth() { return width; }
	s32 getHeight() { return height; }

	void saveToFile(std::string fileName)
	{
		const int w = width, h = height;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		u32 sourceImageSize = h * w * 4;

		u8* screenshot = new u8[sourceImageSize];

		std::string filepath = "screenshot/" + fileName + ".bmp";

		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, screenshot);

		SOIL_save_image(filepath.c_str(), SOIL_SAVE_TYPE_BMP, w, h, 4, screenshot);
	}
};

class GLTextureArray2D : public GLTexture
{
public:
	s32 width, height;
	GLenum sizedFormat;
	GLenum format;
	s32 mipLevels;
	u32 size;
	std::vector<Texture2D*> textures;

	GLTextureArray2D() : width(0), height(0), sizedFormat(0), format(0), mipLevels(0), size(0) {}
	~GLTextureArray2D() {}

	void create(u32 pSize, GLenum pSizedFormat, s32 pWidth, s32 pHeight)
	{
		size = pSize;
		width = pWidth;
		height = pHeight;
		mipLevels = glm::floor(std::log2(glm::max(width, height))) + 1;
		sizedFormat = pSizedFormat;
		textures.resize(size);

		for (int i = 0; i < size; ++i)
		{
			textures[i] = nullptr;
		}

		switch (sizedFormat)
		{
		case GL_RGB8:
			format = GL_RGB;
			break;
		case GL_RGBA8:
			format = GL_RGBA;
			break;
		}

		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &GLID);
		glBindTexture(GL_TEXTURE_2D_ARRAY, GLID);

		glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevels, sizedFormat, width, height, size);
	}

	void setTexture(u32 index, Texture2D* texture)
	{
		if (index + 1 > size)
			return;

		glBindTexture(GL_TEXTURE_2D_ARRAY, GLID);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, width, height, 1, format, GL_UNSIGNED_BYTE, texture->getData());

		textures[index] = texture;
	}

	s32 pushTexture(Texture2D* texture)
	{
		s32 i = 0;
		for (auto t : textures)
		{
			if (!t)
			{
				glBindTexture(GL_TEXTURE_2D_ARRAY, GLID);
				glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, texture->getData());

				textures[i] = texture;
				return i;
			}
			++i;
		}
	}

	void generateMips()
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, GLID);
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	}

	void bind(int pUnit = 0)
	{
		glActiveTexture(GL_TEXTURE0 + pUnit);
		glBindTexture(GL_TEXTURE_2D_ARRAY, GLID);
	}
};

class GLTextureCube : public GLTexture
{
private:
	GLint pixelAlignment;
	GLuint sampler;
	unsigned char* data;
	s32 width, height;

public:
	GLTextureCube() : pixelAlignment(0), sampler(0), data(nullptr) {}
	~GLTextureCube() {}

	void load()
	{
		
	}

	virtual GLuint64 getHandle(GLuint pSampler)
	{
		handle = glGetTextureSamplerHandleARB(GLID, pSampler);
		return handle;
	}
	virtual GLuint64 makeResident(GLuint pSampler)
	{
		if (handle == 0)
		{
			getHandle(pSampler);
		}
		glMakeTextureHandleResidentARB(handle);
		return handle;
	}

	void createFromFiles(const std::vector<std::string>& pFilePaths)
	{
		if (pFilePaths.size() < 6)
			return;

		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &GLID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, GLID);
		unsigned char* image;
		for (int i = 0; i < 6; ++i)
		{
			image = SOIL_load_image(pFilePaths[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8, width, width, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			SOIL_free_image_data(image);
		}
	}

	void createFromStream(GLint pInternalFormat, s32 pWidth, s32 pHeight,GLenum pFormat, GLenum pType, char* image = nullptr, GLint pPixelAlignment = 4)
	{
		sizedFormat = pInternalFormat;
		width = pWidth; height = pHeight;
		format = pFormat;
		type = pType;
		pixelAlignment = pPixelAlignment;
		glPixelStorei(GL_PACK_ALIGNMENT, pixelAlignment);
		glPixelStorei(GL_UNPACK_ALIGNMENT, pixelAlignment);

		glActiveTexture(GL_TEXTURE0);
		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &GLID);
		glBindTextureUnit(0, GLID);

		//glGenTextures(1, &GLID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, GLID);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
		//glTexStorage2D(GL_TEXTURE_CUBE_MAP, 6, pInternalFormat, pWidth, pHeight);

		for (GLuint i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, sizedFormat, width, height, 0, format, type, NULL);

		/*for (int i = 0; i < 6; ++i)
		{
			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, pInternalFormat, width, width, 0, pFormat, pType, 0);
		}*/
	}
	
	void bind(GLint pTextureUnit = 0)
	{
		glBindTextureUnit(pTextureUnit, GLID);
	}

	void bindImage(GLint pImageUnit, GLenum pAccess)
	{
		glBindImageTexture(pImageUnit, GLID, 0, GL_FALSE, 0, pAccess, sizedFormat);
	}

	void saveToFiles(const char* pFilePaths[6])
	{

	}

	s32 getWidth() { return width; }
	s32 getHeight() { return height; }
	glm::ivec2 getResolution() { return glm::ivec2(width, height); }
};

class GLRenderbuffer
{
protected:
	GLint internalFormat;
	GLuint GLID;

public:
	GLRenderbuffer() : internalFormat(0), GLID(0) {}
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

	void destroy()
	{
		glDeleteRenderbuffers(1, &GLID);
	}

	GLuint getGLID() { return GLID; }
};