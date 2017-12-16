#pragma once
#include "Types.hpp"
#include "GLIncludes.hpp"

class GLBufferObject
{
public:
	GLBufferObject() {
		create();
	}
	~GLBufferObject() { 
		destroy();
	}

	void create()
	{
		glCreateBuffers(1, &GLID);
	}

	void destroy()
	{
		glDeleteBuffers(1, &GLID);
	}

	inline void bufferData(GLsizeiptr pSize, void* pData, GLenum pUsage)
	{
		sizeInBytes = pSize;
		glNamedBufferData(GLID, pSize, pData, pUsage);
	}

	inline void bufferSubData(GLintptr pOffset, GLsizeiptr pSize, void* pData)
	{
		glNamedBufferSubData(GLID, pOffset, pSize, pData);
	}

	inline void* mapRange(GLintptr pOffset, GLsizeiptr pLength, GLbitfield pAccess)
	{
		return glMapNamedBufferRange(GLID, pOffset, pLength, pAccess);
	}

	inline void* map(GLbitfield pAccess)
	{
		return glMapNamedBuffer(GLID, pAccess);
	}

	inline void unmap()
	{
		glUnmapNamedBuffer(GLID);
	}

	inline void bindBase(GLenum pBuffer, GLuint pIndex)
	{
		glBindBufferBase(pBuffer, pIndex, GLID);
	}

	inline void bind(GLenum pBuffer)
	{
		glBindBuffer(pBuffer, GLID);
	}

	inline void unbind(GLenum pBuffer)
	{
		glBindBuffer(pBuffer, 0);
	}

	inline void getBufferSubData(GLintptr pOffset, GLsizeiptr pSize, void* pData)
	{
		glGetNamedBufferSubData(GLID, pOffset, pSize, pData);
	}

protected:


	u32 sizeInBytes;
	GLuint GLID;
};

class VBO : public GLBufferObject
{
public:
	VBO() : GLBufferObject() {}
	~VBO() {}

	inline void bindBase(GLuint pIndex, GLenum pBuffer = GL_ARRAY_BUFFER)
	{
		glBindBufferBase(pBuffer, pIndex, GLID);
	}

	inline void bind(GLenum pBuffer = GL_ARRAY_BUFFER)
	{
		glBindBuffer(pBuffer, GLID);
	}

	inline void unbind(GLenum pBuffer = GL_ARRAY_BUFFER)
	{
		glBindBuffer(pBuffer, 0);
	}
};

class SSBO : public GLBufferObject
{
public:
	SSBO() : GLBufferObject() {}
	~SSBO() {}

	inline void bindBase(GLuint pIndex, GLenum pBuffer = GL_SHADER_STORAGE_BUFFER)
	{
		glBindBufferBase(pBuffer, pIndex, GLID);
	}

	inline void bind(GLenum pBuffer = GL_SHADER_STORAGE_BUFFER)
	{
		glBindBuffer(pBuffer, GLID);
	}

	inline void unbind(GLenum pBuffer = GL_SHADER_STORAGE_BUFFER)
	{
		glBindBuffer(pBuffer, 0);
	}
};