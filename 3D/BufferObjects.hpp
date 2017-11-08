#pragma once
#include "Include.hpp"

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

class SSBO : public GLBufferObject
{
public:
	SSBO() : GLBufferObject()
	{
	}
	~SSBO() 
	{
	}
	
	inline void bufferData(GLsizeiptr pSize, void* pData, GLenum pUsage)
	{
		sizeInBytes = pSize;
		glNamedBufferData(GLID, pSize, pData, pUsage);
	}

	inline void* mapRange(GLintptr pOffset, GLsizeiptr pLength, GLbitfield pAccess)
	{
		return glMapNamedBufferRange(GLID, pOffset, pLength, pAccess);
	}

	inline void unmap()
	{
		glUnmapNamedBuffer(GLID);
	}

	inline void bindBase(GLuint pIndex)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, pIndex, GLID);
	}

	inline void bind()
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, GLID);
	}

	inline void unbind()
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

};