#pragma once
#include "Include.h"

class SSBO
{
public:
	SSBO()
	{
		glCreateBuffers(1, &GLID);
	}
	~SSBO()
	{
		glDeleteBuffers(1, &GLID);
	}

	void bufferData(GLsizeiptr pSize, void* pData, GLenum pUsage)
	{
		glNamedBufferData(GLID, pSize, pData, pUsage);
	}

	void* mapRange(GLintptr pOffset, GLsizeiptr pLength, GLbitfield pAccess)
	{
		return glMapNamedBufferRange(GLID, pOffset, pLength, pAccess);
	}

	void unmap()
	{
		glUnmapNamedBuffer(GLID);
	}

	void bindBase(GLuint pIndex)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, pIndex, GLID);
	}

private:

	GLuint GLID;
};