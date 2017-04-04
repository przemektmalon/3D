#pragma once
#include "Include.h"

class SSBO
{
public:
	SSBO()
	{
		glGenBuffers(1, &GLID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, GLID);
	}
	~SSBO()
	{
		glDeleteBuffers(1, &GLID);
	}

	inline void bufferData(GLsizeiptr pSize, void* pData, GLenum pUsage)
	{
		sizeInBytes = pSize;
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, GLID);
		//glBufferData(GL_SHADER_STORAGE_BUFFER, pSize, pData, pUsage);
		glNamedBufferData(GLID, pSize, pData, pUsage);
	}

	inline void* mapRange(GLintptr pOffset, GLsizeiptr pLength, GLbitfield pAccess)
	{
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, GLID);
		//return glMapBufferRange(GL_SHADER_STORAGE_BUFFER, pOffset, pLength, pAccess);
		return glMapNamedBufferRange(GLID, pOffset, pLength, pAccess);
	}

	inline void unmap()
	{
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, GLID);
		//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
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

private:

	u32 sizeInBytes;
	GLuint GLID;
};