#pragma once
#include "Include.hpp"
#include "Texture.hpp"

class DefaultFramebuffer
{
public:
	DefaultFramebuffer() {}
	~DefaultFramebuffer() {}

	void bind() { glBindFramebuffer(GL_FRAMEBUFFER, GLID); }
	void bindRead() { glBindFramebuffer(GL_READ_FRAMEBUFFER, GLID); }
	void bindDraw() { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GLID); }

	void clear(GLbitfield pMask, glm::fvec4 colour = glm::fvec4(0.f))
	{
		glClearColor(colour.r, colour.g, colour.b, colour.a);
		glClear(pMask);
	}

private:
	const GLuint GLID = 0;
};

class Framebuffer
{
public:
	Framebuffer() : created(false) 
	{
		glGenFramebuffers(1, &GLID);
	}
	Framebuffer(glm::ivec2 res) : resolution(res), created(true) 
	{
		glGenFramebuffers(1, &GLID);
	}
	~Framebuffer() 
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
			//std::cout << "A" << std::endl;
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			//throw FramebufferIncompleteException("An attachment could not be bound to frame buffer object!");
			std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" << std::endl;
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			//throw FramebufferIncompleteException("Attachments are missing! At least one image (texture) must be bound to the frame buffer object!");
			std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << std::endl;
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
			//throw FramebufferIncompleteException("The dimensions of the buffers attached to the currently used frame buffer object do not match!");
			std::cout << "GL_FRAMBUFFER_INCOMPLETE_DIMENSIONS" << std::endl;
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
			//throw FramebufferIncompleteException("The formats of the currently used frame buffer object are not supported or do not fit together!");
			std::cout << "GL_FRAMEBUFFER_INCOMPLETE_FORMATS" << std::endl;
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			//throw FramebufferIncompleteException("A Draw buffer is incomplete or undefinied. All draw buffers must specify attachment points that have images attached.");
			std::cout << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" << std::endl;
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			//throw FramebufferIncompleteException("A Read buffer is incomplete or undefinied. All read buffers must specify attachment points that have images attached.");
			std::cout << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" << std::endl;
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			//throw FramebufferIncompleteException("All images must have the same number of multisample samples.");
			std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" << std::endl;
			break;

		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			//throw FramebufferIncompleteException("If a layered image is attached to one attachment, then all attachments must be layered attachments. The attached layers do not have to have the same number of layers, nor do the layers have to come from the same kind of texture.");
			std::cout << "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS" << std::endl;
			break;

		case GL_FRAMEBUFFER_UNSUPPORTED:
			//throw FramebufferIncompleteException("Attempt to use an unsupported format combinaton!");
			std::cout << "GL_FRAMEBUFFER_UNSUPPORTED" << std::endl;
			break;

		default:
			//throw FramebufferIncompleteException("Unknown error while attempting to create frame buffer object!");
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