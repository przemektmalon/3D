#pragma once
#include <unordered_map>
#include "Mesh.h"
#include "Transform.h"

GLfloat skyboxVertices[] = {
	// Positions          
	-10.0f,  10.0f, -10.0f,
	-10.0f, -10.0f, -10.0f,
	10.0f, -10.0f, -10.0f,
	10.0f, -10.0f, -10.0f,
	10.0f,  10.0f, -10.0f,
	-10.0f,  10.0f, -10.0f,

	-10.0f, -10.0f,  10.0f,
	-10.0f, -10.0f, -10.0f,
	-10.0f,  10.0f, -10.0f,
	-10.0f,  10.0f, -10.0f,
	-10.0f,  10.0f,  10.0f,
	-10.0f, -10.0f,  10.0f,

	10.0f, -10.0f, -10.0f,
	10.0f, -10.0f,  10.0f,
	10.0f,  10.0f,  10.0f,
	10.0f,  10.0f,  10.0f,
	10.0f,  10.0f, -10.0f,
	10.0f, -10.0f, -10.0f,

	-10.0f, -10.0f,  10.0f,
	-10.0f,  10.0f,  10.0f,
	10.0f,  10.0f,  10.0f,
	10.0f,  10.0f,  10.0f,
	10.0f, -10.0f,  10.0f,
	-10.0f, -10.0f,  10.0f,

	-10.0f,  10.0f, -10.0f,
	10.0f,  10.0f, -10.0f,
	10.0f,  10.0f,  10.0f,
	10.0f,  10.0f,  10.0f,
	-10.0f,  10.0f,  10.0f,
	-10.0f,  10.0f, -10.0f,

	-10.0f, -10.0f, -10.0f,
	-10.0f, -10.0f,  10.0f,
	10.0f, -10.0f, -10.0f,
	10.0f, -10.0f, -10.0f,
	-10.0f, -10.0f,  10.0f,
	10.0f, -10.0f,  10.0f
};

struct PointLight
{
public:
	PointLight() {}
	PointLight(glm::fvec3 pos, glm::fvec3 col, float lin = 0.0001f, float quad = 0.0003f) : position(pos), colour(col), linear(lin), quadratic(quad) {}
	~PointLight() {}

	glm::fvec3 position, colour;
	float linear, quadratic;
};

struct SpotLight
{
	SpotLight() {}
	SpotLight(glm::fvec3 pos, glm::fvec3 dir, glm::fvec3 col, float inner, float outer = 0.f, float lin = 0.0001f, float quad = 0.0003f) : position(pos), direction(dir), colour(col), linear(lin), quadratic(quad), innerSpread(inner), outerSpread(outer) {
		if (outer = 0.f) { outer = inner; }
	}
	~SpotLight() {}

	glm::fvec3 position, direction, colour;
	float linear, quadratic, innerSpread, outerSpread;
};

struct DirectLight
{
	DirectLight() {}
	DirectLight(glm::fvec3 dir, glm::fvec3 col) : direction(dir), colour(col) {}
	~DirectLight() {}

	glm::fvec3 direction, colour;
};

class MasterRenderer
{
public:
	MasterRenderer() {}
	~MasterRenderer() {}



	inline void initialiseScreenQuad()
	{

		//float bot, left, top, right;
		//bot = ()

		const GLfloat quadVertices[] = {
			-1.0f,  1.0f,  0.0f, 1.0f,
			1.0f,  1.0f,  1.0f, 1.0f,
			1.0f, -1.0f,  1.0f, 0.0f,

			1.0f, -1.0f,  1.0f, 0.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			-1.0f,  1.0f,  0.0f, 1.0f
		};

		glGenVertexArrays(1, &vaoQuad);
		glGenBuffers(1, &vboQuad);

		glBindBuffer(GL_ARRAY_BUFFER, vboQuad);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

		Engine::s.use();

		glBindVertexArray(vaoQuad);
		glBindBuffer(GL_ARRAY_BUFFER, vboQuad);

		GLint posAttrib = glGetAttribLocation(Engine::s(), "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

		GLint texAttrib = glGetAttribLocation(Engine::s(), "texCoord");
		glEnableVertexAttribArray(texAttrib);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

		glUniform1i(glGetUniformLocation(Engine::s(), "gPosition"), 0);
		glUniform1i(glGetUniformLocation(Engine::s(), "gNormal"), 1);
		glUniform1i(glGetUniformLocation(Engine::s(), "gAlbedoSpec"), 2);

		Engine::s.stop();
	}

	inline void initialiseGBuffer()
	{
		/*
		gBufferShader.load("res/shader/gBufferPass", "res/shader/gBufferPass");

		glGenFramebuffers(1, &fboGBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, fboGBuffer);

		glGenTextures(3, texAttachGBuffer);

		for (int i = 0; i < 2; ++i)
		{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texAttachGBuffer[i]);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAALevel, GL_RGB16F, int(viewport.width * frameScale), int(viewport.height * frameScale), GL_TRUE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, texAttachGBuffer[i], 0);
		}

		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texAttachGBuffer[2]);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAALevel, GL_RGBA16F, int(viewport.width * frameScale), int(viewport.height * frameScale), GL_TRUE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D_MULTISAMPLE, texAttachGBuffer[2], 0);

		GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		glGenRenderbuffers(1, &rboDepthStencilGBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencilGBuffer);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAALevel, GL_DEPTH24_STENCIL8, int(viewport.width * frameScale), int(viewport.height * frameScale));
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencilGBuffer);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Error: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		*/

		gBufferShader.load("res/shader/gBufferPass", "res/shader/gBufferPass");

		glGenFramebuffers(1, &fboGBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, fboGBuffer);

		glGenTextures(3, texAttachGBuffer);

		for (int i = 0; i < 2; ++i)
		{
			glBindTexture(GL_TEXTURE_2D, texAttachGBuffer[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, int(viewport.width * frameScale), int(viewport.height * frameScale), 0, GL_RGB, GL_FLOAT, NULL);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texAttachGBuffer[i], 0);
		}

		glBindTexture(GL_TEXTURE_2D, texAttachGBuffer[2]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, int(viewport.width * frameScale), int(viewport.height * frameScale), 0, GL_RGBA, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, texAttachGBuffer[2], 0);

		GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		glGenRenderbuffers(1, &rboDepthStencilGBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencilGBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, int(viewport.width * frameScale), int(viewport.height * frameScale));
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencilGBuffer);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Error: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	inline void initialiseScreenFramebuffer()
	{
		glGenFramebuffers(1, &fboScreen);
		glBindFramebuffer(GL_FRAMEBUFFER, fboScreen);

		glGenTextures(3, texAttachScreen);
		glBindTexture(GL_TEXTURE_2D, texAttachScreen[0]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, viewport.width, viewport.height, 0, GL_RGB, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texAttachScreen[0], 0);

		glBindTexture(GL_TEXTURE_2D, texAttachScreen[1]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, viewport.width, viewport.height, 0, GL_RGB, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texAttachScreen[1], 0);

		glBindTexture(GL_TEXTURE_2D, texAttachScreen[2]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, viewport.width, viewport.height, 0, GL_RGBA, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, texAttachScreen[2], 0);

		glGenRenderbuffers(1, &rboDepthStencilScreen);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencilScreen);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, viewport.width, viewport.height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencilScreen);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Error: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	inline void initialisePostLightingFramebuffer()
	{
		glGenFramebuffers(1, &fboPostLighting);
		glBindFramebuffer(GL_FRAMEBUFFER, fboPostLighting);

		glGenTextures(1, &texAttachPostLighting);
		glBindTexture(GL_TEXTURE_2D, texAttachPostLighting);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, viewport.width, viewport.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texAttachPostLighting, 0);

		glGenRenderbuffers(1, &rboDepthStencilPostLighting);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencilPostLighting);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, viewport.width, viewport.height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencilPostLighting);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Error: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	inline void initialiseSkybox()
	{
		skyboxShader.load("res/shader/skybox", "res/shader/skybox"); glUseProgram(skyboxShader());

		glGenVertexArrays(1, &vaoSkybox);
		glGenBuffers(1, &vboSkybox);
		glBindVertexArray(vaoSkybox);
		glBindBuffer(GL_ARRAY_BUFFER, vaoSkybox);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glBindVertexArray(0);

		glUniform1i(glGetUniformLocation(skyboxShader(), "skybox"), 5);

		glGenTextures(1, &skyboxTex);

		int width, height;
		unsigned char* image;

		std::string skyboxPath = "res/skybox/morning/";

		std::vector<std::string> faces;

		faces.push_back(skyboxPath + "right.png");
		faces.push_back(skyboxPath + "left.png");
		faces.push_back(skyboxPath + "top.png");
		faces.push_back(skyboxPath + "bottom.png");
		faces.push_back(skyboxPath + "back.png");
		faces.push_back(skyboxPath + "front.png");

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
		for (GLuint i = 0; i < 6; i++)
		{
			image = SOIL_load_image(faces[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			//glGenerateMipmap(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			SOIL_free_image_data(image);
		}
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	inline void initialiseLights()
	{
		const int nr = 5;
		for (int i = 0; i < nr - 1; ++i)
		{
			PointLight light;
			light.position = glm::fvec3(float(Engine::rand() % 1000) - 500, float(Engine::rand() % 400), float(Engine::rand() % 500) - 100);
			light.colour = glm::fvec3((Engine::rand() % 70) + 200, (Engine::rand() % 70) + 200, (Engine::rand() % 700) + 200);
			light.colour = glm::normalize(light.colour); light.colour *= glm::fvec3(1.f);
			light.linear = 0.00001f;
			light.quadratic = 0.000011f;
			pointLights.push_back(light);
		}

		PointLight light;
		light.position = glm::fvec3(-400, 10, -400);
		light.colour = glm::fvec3(100, 100, 100);
		light.linear = 0.0001f;
		light.quadratic = 0.001f;
		pointLights.push_back(light);



		Engine::s.use();

		SpotLight spot = SpotLight(glm::fvec3(200, 200, 200), glm::fvec3(0, -0.2, 0.f), glm::fvec3(0.f, 0.f, 100.f), 45, 50, 0.00001, 0.00003);
		SpotLight spot2 = SpotLight(glm::fvec3(150, 200, 150), glm::fvec3(0, -0.2, 0.f), glm::fvec3(0.f, 100.f, 0.f), 45, 50, 0.00001, 0.00003);
		SpotLight spot3 = SpotLight(glm::fvec3(150, 200, 250), glm::fvec3(0, -0.2, 0.f), glm::fvec3(100.f, 0.f, 0.f), 45, 50, 0.00001, 0.00003);

		spotLights.push_back(spot);
		spotLights.push_back(spot2);
		spotLights.push_back(spot3);

		for (int i = 0; i < spotLights.size(); ++i)
		{
			glUniform3fv(glGetUniformLocation(Engine::s(), ("spotLights[" + std::to_string(i) + "].position").c_str()), 1, &spotLights[i].position[0]);
			glUniform3fv(glGetUniformLocation(Engine::s(), ("spotLights[" + std::to_string(i) + "].colour").c_str()), 1, &spotLights[i].colour[0]);
			glUniform3fv(glGetUniformLocation(Engine::s(), ("spotLights[" + std::to_string(i) + "].direction").c_str()), 1, &spotLights[i].direction[0]);
			glUniform1f(glGetUniformLocation(Engine::s(), ("spotLights[" + std::to_string(i) + "].innerSpread").c_str()), glm::cos(glm::radians(spotLights[i].innerSpread)));
			glUniform1f(glGetUniformLocation(Engine::s(), ("spotLights[" + std::to_string(i) + "].outerSpread").c_str()), glm::cos(glm::radians(spotLights[i].outerSpread)));
			glUniform1f(glGetUniformLocation(Engine::s(), ("spotLights[" + std::to_string(i) + "].linear").c_str()), spotLights[i].linear);
			glUniform1f(glGetUniformLocation(Engine::s(), ("spotLights[" + std::to_string(i) + "].quadratic").c_str()), spotLights[i].quadratic);
		}

		DirectLight dir = DirectLight(glm::fvec3(-0.1, -1, -0.1), glm::fvec3(1.f, 1.f, 0.75f));

		glUniform3fv(glGetUniformLocation(Engine::s(), "directLights[0].direction"), 1, &dir.direction[0]);
		glUniform3fv(glGetUniformLocation(Engine::s(), "directLights[0].colour"), 1, &dir.colour[0]);

		float xx = 0b1'01111100'01000000000000000000000;

		std::cout << xx << std::endl;

		for (int i = 0; i < nr; ++i)
		{
			glUniform3fv(glGetUniformLocation(Engine::s(), ("pointLights[" + std::to_string(i) + "].position").c_str()), 1, &pointLights[i].position[0]);
			glUniform3fv(glGetUniformLocation(Engine::s(), ("pointLights[" + std::to_string(i) + "].colour").c_str()), 1, &pointLights[i].colour[0]);
			glUniform1f(glGetUniformLocation(Engine::s(), ("pointLights[" + std::to_string(i) + "].linear").c_str()), pointLights[i].linear);
			glUniform1f(glGetUniformLocation(Engine::s(), ("pointLights[" + std::to_string(i) + "].quadratic").c_str()), pointLights[i].quadratic);
		}
	}

	void initialiseRenderer(int Msaalev, Window* pwin)
	{
		window = pwin;
		MSAALevel = Msaalev;
		viewport.bot = 0; viewport.left = 0; viewport.width = window->getSizeX(); viewport.height = window->getSizeY();
		frameScale = 1.f;

		initialiseLights();
		initialiseScreenQuad();
		initialiseGBuffer();
		initialiseScreenFramebuffer();
		initialisePostLightingFramebuffer();
		initialiseSkybox();
	}

	void render(Camera& cam)
	{
		glViewport(viewport.left, viewport.left, viewport.width, viewport.height);
		glBindFramebuffer(GL_FRAMEBUFFER, fboGBuffer);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		glUseProgram(gBufferShader());
		auto ploc = glGetUniformLocation(gBufferShader(), "proj");
		auto mloc = glGetUniformLocation(gBufferShader(), "model");
		glUniformMatrix4fv(ploc, 1, GL_FALSE, glm::value_ptr(cam.projView));

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		if (Engine::wf)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		for (auto itr = entities.begin(); itr != entities.end(); ++itr)
		{
			glBindVertexArray(itr->first->vao);
			glBindBuffer(GL_ARRAY_BUFFER, itr->first->vbo);
			for (auto itr2 = itr->second.begin(); itr2 != itr->second.end(); ++itr2)
			{
				glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(*itr2));
				glDrawArrays(GL_TRIANGLES, 0, itr->first->data.numVert);
			}
		}

		if (Engine::wf)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboScreen);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fboGBuffer);

		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(0, 0, window->getSizeX(), window->getSizeY(), 0, 0, window->getSizeX(), window->getSizeY(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glReadBuffer(GL_COLOR_ATTACHMENT1);
		glDrawBuffer(GL_COLOR_ATTACHMENT1);
		glBlitFramebuffer(0, 0, window->getSizeX(), window->getSizeY(), 0, 0, window->getSizeX(), window->getSizeY(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glReadBuffer(GL_COLOR_ATTACHMENT2);
		glDrawBuffer(GL_COLOR_ATTACHMENT2);
		glBlitFramebuffer(0, 0, window->getSizeX(), window->getSizeY(), 0, 0, window->getSizeX(), window->getSizeY(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

		//glReadBuffer(GL_DEPTH_STENCIL_ATTACHMENT);
		//glDrawBuffer(GL_DEPTH_STENCIL_ATTACHMENT);
		//glBlitFramebuffer(0, 0, window->getSizeX(), window->getSizeY(), 0, 0, window->getSizeX(), window->getSizeY(), GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);

		//TODO: POST PROCESSING ON fboScreen

		glUseProgram(Engine::s());
		glBindFramebuffer(GL_FRAMEBUFFER, fboPostLighting);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		spotLights[0].position = cam.pos;
		spotLights[0].direction = cam.getDirectionVector();

		glUniform3fv(glGetUniformLocation(Engine::s(), "spotLights[0].position"), 1, &spotLights[0].position[0]);
		glUniform3fv(glGetUniformLocation(Engine::s(), "spotLights[0].direction"), 1, &spotLights[0].direction[0]);

		auto posloc = glGetUniformLocation(Engine::s(), "viewPos");
		glUniform3f(posloc, cam.pos.x, cam.pos.y, cam.pos.z);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		glBindVertexArray(vaoQuad);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texAttachScreen[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texAttachScreen[1]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texAttachScreen[2]);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);

		//RENDER SKYBOX *********************************************************************

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fboPostLighting);

		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glDrawBuffer(GL_BACK);
		glBlitFramebuffer(0, 0, window->getSizeX(), window->getSizeY(), 0, 0, window->getSizeX(), window->getSizeY(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fboGBuffer);

		glReadBuffer(GL_DEPTH_STENCIL_ATTACHMENT);
		glDrawBuffer(GL_DEPTH_ATTACHMENT);
		glBlitFramebuffer(0, 0, window->getSizeX(), window->getSizeY(), 0, 0, window->getSizeX(), window->getSizeY(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		glUseProgram(skyboxShader());

		glDrawBuffer(GL_BACK);

		glDepthFunc(GL_LEQUAL);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glm::mat4 view = cam.rotation;
		glm::mat4 projection = cam.proj; // glm::infinitePerspective(cam.fov, cam.aspect, 1.f);
		glUseProgram(skyboxShader());
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader(), "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(vaoSkybox);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
		glUniform1i(glGetUniformLocation(skyboxShader(), "skybox"), 5);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);

		glDepthFunc(GL_LESS);

		//RENDER SKYBOX *********************************************************************

		window->swapBuffers();
	}

	//GLuint screenShaderProgram, screenShaderProgram2;

	template <class T>
	struct Rect
	{
		Rect() {}
		~Rect() {}

		T bot, left, width, height;
	};

	std::unordered_map<Mesh*, std::vector<glm::fmat4>> entities;
	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;
	std::vector<DirectLight> directLights;

	Rect<int> viewport;
	float frameScale;

	Window* window;
	int MSAALevel;
	GLuint fboGBuffer, fboScreen, fboPostLighting;
	GLuint texAttachGBuffer[3];
	GLuint texAttachScreen[3];
	GLuint texAttachPostLighting;
	GLuint rboDepthStencilGBuffer, rboDepthStencilScreen, rboDepthStencilPostLighting;
	GLuint vaoQuad;
	GLuint vboQuad;
	Shader toScreenShader; //TODO: SPECIFIC SHADERS

	Shader skyboxShader;
	GLuint skyboxTex;
	GLuint vaoSkybox, vboSkybox;

	Shader gBufferShader;
};