#pragma once
#include <unordered_map>
#include "Mesh.h"
#include "Transform.h"

const GLfloat quadVertices2[] = {
	-1.0f,  1.0f,  0.0f, 1.0f,
	1.0f,  1.0f,  1.0f, 1.0f,
	1.0f, -1.0f,  1.0f, 0.0f,

	1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
};

class MasterRenderer
{
public:
	MasterRenderer() {}
	~MasterRenderer() {}

	std::unordered_map<Mesh*, std::vector<glm::fmat4>> entities;

	void initialiseRenderer(int Msaalev, Window* pwin)
	{
		window = pwin;
		MSAALevel = Msaalev;

		glGenVertexArrays(1, &vaoQuad);
		glGenBuffers(1, &vboQuad);

		glBindBuffer(GL_ARRAY_BUFFER, vboQuad);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices2), quadVertices2, GL_STATIC_DRAW);

		Shader post1;
		post1.load("res/shader/GaussBlurH", "res/shader/GaussBlur");
		Shader post2;
		post2.load("res/shader/GaussBlurV", "res/shader/GaussBlur");

		auto bradloc = glGetUniformLocation(post1(), "blurRadius");

		screenShaderProgram = post1();
		screenShaderProgram2 = post2();

		glUseProgram(post1());

		glBindVertexArray(vaoQuad);
		glBindBuffer(GL_ARRAY_BUFFER, vboQuad);

		GLint posAttrib = glGetAttribLocation(screenShaderProgram, "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

		GLint texAttrib = glGetAttribLocation(screenShaderProgram, "texcoord");
		glEnableVertexAttribArray(texAttrib);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

		glUseProgram(screenShaderProgram);
		glUniform1i(glGetUniformLocation(screenShaderProgram, "s_texture"), 4);

		glUseProgram(screenShaderProgram2);
		glUniform1i(glGetUniformLocation(screenShaderProgram2, "s_texture"), 4);

		glGenFramebuffers(1, &fboMSAA);
		glBindFramebuffer(GL_FRAMEBUFFER, fboMSAA);

		glGenTextures(1, &texColourBufferMSAA);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texColourBufferMSAA);

		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAALevel, GL_RGBA8, window->getSizeX(), window->getSizeY(), GL_TRUE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texColourBufferMSAA, 0);

		glGenRenderbuffers(1, &rboDepthStencilMSAA);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencilMSAA);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAALevel, GL_DEPTH24_STENCIL8, window->getSizeX(), window->getSizeY());
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencilMSAA);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Error: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glGenFramebuffers(1, &fboScreen);
		glBindFramebuffer(GL_FRAMEBUFFER, fboScreen);

		glGenTextures(1, &texColourBufferScreen);
		glBindTexture(GL_TEXTURE_2D, texColourBufferScreen);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, window->getSizeX(), window->getSizeY(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColourBufferScreen, 0);

		glGenRenderbuffers(1, &rboDepthStencilScreen);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencilScreen);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window->getSizeX(), window->getSizeY());
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencilScreen);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Error: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void render(Camera& cam)
	{
		glUseProgram(Engine::s());
		glBindFramebuffer(GL_FRAMEBUFFER, fboMSAA);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.01f, 0.01f, 0.01f, 1.0f);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		glUniform3f(0, cam.pos.x, cam.pos.y, cam.pos.z);
		glUniformMatrix4fv(6, 1, GL_FALSE, glm::value_ptr(cam.projView));

		for (auto itr = entities.begin(); itr != entities.end(); ++itr)
		{
			glBindVertexArray(itr->first->vao);
			glBindBuffer(GL_ARRAY_BUFFER, itr->first->vbo);
			for (auto itr2 = itr->second.begin(); itr2 != itr->second.end(); ++itr2)
			{
				glUniformMatrix4fv(7, 1, GL_FALSE, glm::value_ptr(*itr2));
				glDrawArrays(GL_TRIANGLES, 0, itr->first->data.numVert);
			}
		}

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboScreen);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fboMSAA);
		glReadBuffer(GL_COLOR_ATTACHMENT0);//TODO: ADD SUPPORT FOR MULTIPLE COLOUR ATTACHMENTS TO ALLOW DEFERRED SHADING/LIGHTING
		glDrawBuffer(GL_BACK);
		glBlitFramebuffer(0, 0, window->getSizeX(), window->getSizeY(), 0, 0, window->getSizeX(), window->getSizeY(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

		//TODO: POST PROCESSING ON fboScreen

		glUseProgram(screenShaderProgram);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		float blurRadius = 0.f;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindVertexArray(vaoQuad);

		auto bradloc = glGetUniformLocation(screenShaderProgram, "blurRadius");

		glUniform1f(bradloc, (1.f / float(window->getSizeY())) * blurRadius); //V

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, texColourBufferScreen);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);

		window->swapBuffers();
	}

	GLuint screenShaderProgram, screenShaderProgram2;

	Window* window;
	int MSAALevel;
	GLuint fboMSAA, fboScreen;
	GLuint texColourBufferMSAA, texColourBufferScreen;
	GLuint rboDepthStencilMSAA, rboDepthStencilScreen;
	GLuint vaoQuad;
	GLuint vboQuad;
	Shader toScreenShader; //TODO: SPECIFIC SHADERS
};