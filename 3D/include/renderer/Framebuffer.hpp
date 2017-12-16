#pragma once
#include "Texture.hpp"
#include <iostream>

class DefaultFBO
{
public:
	DefaultFBO() {}
	~DefaultFBO() {}

	static void bind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
	static void bindRead() { glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); }
	static void bindDraw() { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); }

	static void clear(GLbitfield pMask, glm::fvec4 colour = glm::fvec4(0.f))
	{
		glClearColor(colour.r, colour.g, colour.b, colour.a);
		glClear(pMask);
	}

};

class FBO
{
public:
	FBO() : created(false) 
	{
		glGenFramebuffers(1, &GLID);
	}
	FBO(glm::ivec2 res) : resolution(res), created(true) 
	{
		glGenFramebuffers(1, &GLID);
	}
	~FBO() 
	{
		glDeleteFramebuffers(1, &GLID);
	}

	void setClearDepth(float pDepth)
	{
		clearDepth = pDepth;
	}

	void clear(GLbitfield pMask, glm::fvec4 colour = glm::fvec4(0.f))
	{
		glClearColor(colour.r, colour.g, colour.b, colour.a);
		//glClearDepth(clearDepth);
		//glClearDepthdNV(clearDepth);
		glClear(pMask);
	}

	void setResolution(glm::ivec2 res)
	{
		created = true;
		resolution = res;
	}

	/*InernalFormat == Number of colour components
	Format == Pixcel formatting
	Type == Type used to store components*/
	int attachTexture(GLint pInternalFormat, GLint pFormat, GLenum pType, GLenum pAttachment, glm::fvec2 scale = glm::fvec2(1.f,1.f))
	{
		bind();

		GLTexture2D attachment;
		attachment.createFromStream(pInternalFormat, resolution.x * scale.x, resolution.y * scale.y, pFormat, pType, NULL);

		attachment.bind();

		glFramebufferTexture2D(GL_FRAMEBUFFER, pAttachment, GL_TEXTURE_2D, attachment.getGLID(), 0);

		textureAttachments.push_back(attachment);
		return attachment.getGLID();
	}

	bool attachForeignTexture(GLTexture2D* pTexture, GLenum pAttachment)
	{
		bind();
		pTexture->bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, pAttachment, GL_TEXTURE_2D, pTexture->getGLID(), 0);
		return true; ///TODO: or flase if error occurred, i.e. incomplete attachment texture
	}

	bool attachForeignCubeTexture(GLTextureCube* pTexture, GLenum pAttachment)
	{
		bind();
		pTexture->bind();
		glFramebufferTexture(GL_FRAMEBUFFER, pAttachment, pTexture->getGLID(), 0);
		return true;
	}

	int attachRenderbuffer(GLint pInternalFormat, GLenum pAttachment, glm::fvec2 scale = glm::fvec2(1.f, 1.f))
	{
		bind();

		renderbufferAttachments.push_back(GLRenderbuffer());

		auto& attachment = renderbufferAttachments.back();
		attachment.create(pInternalFormat, resolution.x * scale.x, resolution.y * scale.y);

		attachment.bind();

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, pAttachment, GL_RENDERBUFFER, attachment.getGLID());

		return attachment.getGLID();
	}

	void destroyAllAttachments()
	{
		bind();
		for (auto itr = textureAttachments.begin(); itr != textureAttachments.end(); ++itr)
		{
			itr->release();
		}
		for (auto itr = renderbufferAttachments.begin(); itr != renderbufferAttachments.end(); ++itr)
		{
			//itr->release();
		}
		textureAttachments.clear();
	}

	GLuint checkStatus()
	{
		GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		switch (status) {
		case GL_FRAMEBUFFER_COMPLETE:
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << std::endl;
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << std::endl;
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			std::cout << "GL_FRAMBUFFER_INCOMPLETE_DIMENSIONS" << std::endl;
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			std::cout << "GL_FRAMEBUFFER_INCOMPLETE_FORMATS" << std::endl;
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			std::cout << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" << std::endl;
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			std::cout << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" << std::endl;
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" << std::endl;
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			std::cout << "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS" << std::endl;
			break;

		case GL_FRAMEBUFFER_UNSUPPORTED:
			std::cout << "GL_FRAMEBUFFER_UNSUPPORTED" << std::endl;
			break;

		default:
			std::cout << "Unkown framebuffer status" << std::endl;
			break;
		}
		return status;
	}

	void bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, GLID);
	}
	void bindDraw()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GLID);
	}
	void bindRead()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, GLID);
	}

	bool created;

	GLuint GLID;
	std::vector<GLTexture2D> textureAttachments;
	std::vector<GLRenderbuffer> renderbufferAttachments;
	GLuint depthAttachment;

	glm::ivec2 resolution;
	glm::fvec4 clearColour;
	float clearDepth;
};