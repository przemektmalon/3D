#pragma once
#include "Include.hpp"

class Sampler
{
public:
	Sampler() { glGenSamplers(1, &GLID); }
	~Sampler() { glDeleteSamplers(1, &GLID); }

	void bind(GLuint pImageUnit)
	{
		glBindSampler(pImageUnit, GLID);
	}

	GLuint getGLID() { return GLID; }

	void initialiseDefaults()
	{
		setDepthStencilTextureMode(GL_DEPTH_COMPONENT);
		setTextureBaseLevel(0);
		setTextureCompareFunc(GL_LEQUAL);
		setTextureCompareMode(GL_NONE);
		setTextureLODBias(0.f);
		setTextureMinFilter(GL_NEAREST_MIPMAP_LINEAR);
		setTextureMagFilter(GL_LINEAR);
		setTextureMinLOD(-1000);
		setTextureMaxLOD(1000);
		setTextureMaxLevel(1000);
		setTextureSwizzle(GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA);
		setTextureWrapS(GL_REPEAT);
		setTextureWrapT(GL_REPEAT);
		setTextureWrapR(GL_REPEAT);
		setTextureBorderColour(0.f, 0.f, 0.f, 0.f);
	}

	void setDepthStencilTextureMode(GLenum pParam) { glSamplerParameteri(GLID, GL_DEPTH_STENCIL_TEXTURE_MODE, pParam); }
	void setTextureBaseLevel(GLuint pParam) { glSamplerParameteri(GLID, GL_TEXTURE_BASE_LEVEL, pParam); }
	void setTextureCompareFunc(GLenum pParam) { glSamplerParameteri(GLID, GL_TEXTURE_COMPARE_FUNC, pParam); }
	void setTextureCompareMode(GLenum pParam) { glSamplerParameteri(GLID, GL_TEXTURE_COMPARE_MODE, pParam); }
	void setTextureLODBias(GLfloat pParam) { glSamplerParameterf(GLID, GL_TEXTURE_LOD_BIAS, pParam); }
	void setTextureMinFilter(GLenum pParam) {
		glSamplerParameteri(GLID, GL_TEXTURE_MIN_FILTER, pParam);
	}
	void setTextureMagFilter(GLenum pParam) { glSamplerParameteri(GLID, GL_TEXTURE_MAG_FILTER, pParam); }
	void setTextureMinLOD(GLuint pParam) { glSamplerParameteri(GLID, GL_TEXTURE_MIN_LOD, pParam); }
	void setTextureMaxLOD(GLuint pParam) { glSamplerParameteri(GLID, GL_TEXTURE_MAX_LOD, pParam); }
	void setTextureMaxLevel(GLuint pParam) { glSamplerParameteri(GLID, GL_TEXTURE_MAX_LEVEL, pParam); }
	void setTextureSwizzleRed(GLenum pParam) { glSamplerParameteri(GLID, GL_TEXTURE_SWIZZLE_R, pParam); }
	void setTextureSwizzleGreen(GLenum pParam) { glSamplerParameteri(GLID, GL_TEXTURE_SWIZZLE_G, pParam); }
	void setTextureSwizzleBlue(GLenum pParam) { glSamplerParameteri(GLID, GL_TEXTURE_SWIZZLE_B, pParam); }
	void setTextureSwizzleAlpha(GLenum pParam) { glSamplerParameteri(GLID, GL_TEXTURE_SWIZZLE_A, pParam); }
	void setTextureSwizzle(GLenum pR, GLenum pG, GLenum pB, GLenum pA)
	{
		glSamplerParameteri(GLID, GL_DEPTH_STENCIL_TEXTURE_MODE, pR);
		glSamplerParameteri(GLID, GL_DEPTH_STENCIL_TEXTURE_MODE, pG);
		glSamplerParameteri(GLID, GL_DEPTH_STENCIL_TEXTURE_MODE, pB);
		glSamplerParameteri(GLID, GL_DEPTH_STENCIL_TEXTURE_MODE, pA);
	}
	void setTextureWrapS(GLenum pParam) { glSamplerParameteri(GLID, GL_TEXTURE_WRAP_S, pParam); }
	void setTextureWrapT(GLenum pParam) { glSamplerParameteri(GLID, GL_TEXTURE_WRAP_T, pParam); }
	void setTextureWrapR(GLenum pParam) { glSamplerParameteri(GLID, GL_TEXTURE_WRAP_R, pParam); }
	void setTextureBorderColour(GLfloat pR, GLfloat pG, GLfloat pB, GLfloat pA)
	{
		float cols[] = { pR, pG, pB, pA };
		glSamplerParameterfv(GLID, GL_TEXTURE_BORDER_COLOR, cols);
	}
	void setTextureAnisotropy(GLfloat pParam) { glSamplerParameterf(GLID, GL_TEXTURE_MAX_ANISOTROPY_EXT, pParam); }

private:

	/*GLenum depthStencilTextureMode;
	GLuint textureBaseLevel;
	GLenum textureCompareFunc;
	GLenum textureCompareMode;
	GLfloat textureLODBias;
	GLenum textureMinFilter;
	GLenum textureMagFilter;
	GLuint textureMinLOD;
	GLuint textureMaxLOD;
	GLuint textureMaxLevel;
	GLenum textureSwizzleRed;
	GLenum textureSwizzleBlue;
	GLenum textureSwizzleGreen;
	GLenum textureSwizzleAlpha;
	GLenum textureWrapS;
	GLenum textureWrapT;
	GLenum textureWrapR;
	GLfloat borderR;
	GLfloat borderG;
	GLfloat borderB;
	GLfloat borderA;*/

	GLuint GLID;
};