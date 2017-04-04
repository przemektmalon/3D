#pragma once
#include <unordered_map>
#include "Mesh.h"
#include "Transform.h"
#include "SAOShader.h"
#include "GBufferShader.h"
#include "Framebuffer.h"
#include "Text.h"

#include "Lights.h"

class MasterRenderer
{
public:
	MasterRenderer() : ssaoPower(1.f), ssaoScale(1.f), shadScale(1.f) {}
	~MasterRenderer() {}

	inline void initialiseScreenQuad()
	{
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
		auto ccc = glGetUniformLocation(Engine::s(), "gAlbedoSpec");
		glUniform1i(ccc, 2);
		//glUniform1i(glGetUniformLocation(Engine::s(), "gDepth"), 3);
		glUniform1i(glGetUniformLocation(Engine::s(), "ssaoTex"), 3);
		glUniform1i(glGetUniformLocation(Engine::s(), "shadow"), 8);

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



		//glm::fvec4 projInfo(-2.f / (viewport.width * cam.proj[0][0]), -2.f / (viewport.height*cam.proj[1][1]), (1.f - cam.proj[0][2]) / cam.proj[0][0], (1.f + cam.proj[1][2]) / cam.proj[1][1]);
		//glUniform4fv(glGetUniformLocation(ssaoShader(), "projInfo"), 1, glm::value_ptr(projInfo));


		//gBufferShader.load("res/shader/gBufferPass", "res/shader/gBufferPass");

		fboGBuffer.setResolution(glm::fvec2(window->getSize()) * frameScale);
		fboGBuffer.attachTexture(GL_RGB32F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT0);
		fboGBuffer.attachTexture(GL_RGB32F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT1);
		fboGBuffer.attachTexture(GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT2);
		fboGBuffer.attachTexture(GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
		fboGBuffer.checkStatus();

		GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		fboSSAO.setResolution(glm::fvec2(window->getSize()) * frameScale);
		fboSSAO.attachTexture(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0, glm::fvec2(ssaoScale));
		fboSSAO.checkStatus();

		fboSSAOBlur.setResolution(glm::fvec2(window->getSize()) * frameScale);
		fboSSAOBlur.attachTexture(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0, glm::fvec2(ssaoScale));
		fboSSAOBlur.checkStatus();

		blurShader.load("res/shader/Standard", "res/shader/bilatBlur"); blurShader.use();
		auto locc = glGetUniformLocation(blurShader(), "source");
		glUniform1i(locc, 0);
		blurShader.stop();
	}

	inline void initialiseScreenFramebuffer()
	{
		fboScreen.setResolution(glm::fvec2(window->getSize()) * frameScale);
		fboScreen.attachTexture(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0);
		fboScreen.checkStatus();
	}

	inline void initialiseSkybox()
	{
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

		std::string skyboxPath = "res/skybox/sky/";

		std::vector<std::string> faces;

		faces.push_back(skyboxPath + "right.png");
		faces.push_back(skyboxPath + "left.png");
		faces.push_back(skyboxPath + "top.png");
		faces.push_back(skyboxPath + "bottom.png");
		faces.push_back(skyboxPath + "front.png");
		faces.push_back(skyboxPath + "back.png");

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
			light.colour = glm::normalize(light.colour); light.colour *= glm::fvec3(0.2f);
			light.linear = 0.0001f;
			light.quadratic = 0.000011f;
			light.colour = glm::fvec3(0.1, 0.2, 0.5);
			pointLights.push_back(light);
		}

		PointLight light;
		light.position = glm::fvec3(-400, 10, -400);
		light.colour = glm::fvec3(100, 100, 100);
		light.linear = 0.0001f;
		light.quadratic = 0.001f;
		pointLights.push_back(light);



		Engine::s.use();

		SpotLight spot = SpotLight(glm::fvec3(200, 200, 200), glm::fvec3(-PI / 4 - 0.3, -0.2, 0.f), glm::fvec3(0.f, 0.f, 5.f), 10, 30, 0.00001, 0.00003);
		SpotLight spot2 = SpotLight(glm::fvec3(150, 200, 150), glm::fvec3(-PI / 4, -0.2, 0.f), glm::fvec3(0.f, 5.f, 0.f), 10, 30, 0.00001, 0.00003);
		SpotLight spot3 = SpotLight(glm::fvec3(150, 200, 250), glm::fvec3(-PI / 4, -0.2, 0.f), glm::fvec3(5.f, 0.f, 0.f), 10, 30, 0.00001, 0.00003);

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

		glm::fvec3 ddir(-2, -2, -4);
		ddir = glm::normalize(ddir);

		//glm::fvec3 ddir = glm::fvec3(glm::fvec4(1.f, -1.f, -1.f, 1.f));
		DirectLight dir = DirectLight(ddir, glm::fvec3(1.f, 1.f, 1.25f));

		glUniform3fv(glGetUniformLocation(Engine::s(), "directLights[0].direction"), 1, &dir.direction[0]);
		glUniform3fv(glGetUniformLocation(Engine::s(), "directLights[0].colour"), 1, &dir.colour[0]);

		for (int i = 0; i < nr; ++i)
		{
			glUniform3fv(glGetUniformLocation(Engine::s(), ("pointLights[" + std::to_string(i) + "].position").c_str()), 1, &pointLights[i].position[0]);
			glUniform3fv(glGetUniformLocation(Engine::s(), ("pointLights[" + std::to_string(i) + "].colour").c_str()), 1, &pointLights[i].colour[0]);
			glUniform1f(glGetUniformLocation(Engine::s(), ("pointLights[" + std::to_string(i) + "].linear").c_str()), pointLights[i].linear);
			glUniform1f(glGetUniformLocation(Engine::s(), ("pointLights[" + std::to_string(i) + "].quadratic").c_str()), pointLights[i].quadratic);
		}

		shadowShader.load("res/shader/shadowPass", "res/shader/shadowPass"); shadowShader.use();

		auto posAttrib = glGetAttribLocation(shadowShader(), "p");
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(posAttrib);

		fboLight.setResolution(glm::fvec2(shadRes) * frameScale * shadScale);
		//fboLight.attachTexture(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0, glm::fvec2(ssaoScale));
		fboLight.attachTexture(GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		fboLight.checkStatus();
	}

	void initialiseRenderer(int Msaalev, Window* pwin)
	{
		window = pwin;
		MSAALevel = Msaalev;
		viewport.bot = 0; viewport.left = 0; viewport.width = window->getSizeX(); viewport.height = window->getSizeY();
		frameScale = 1.f;
		renderResolution.x = viewport.width * frameScale;
		renderResolution.y = viewport.height * frameScale;

		initialiseLights();
		initialiseScreenQuad();
		initialiseGBuffer();
		initialiseScreenFramebuffer();
		initialiseSkybox();
	}

	void setActiveCam(Camera& cam)
	{
		ssaoShader.setProj(cam.proj, glm::ivec2(viewport.width, viewport.height));
	}

	void render(Camera& cam)
	{
		// *********************************************************** G-BUFFER PASS *********************************************************** //

		glViewport(0, 0, renderResolution.x, renderResolution.y);

		fboGBuffer.bind();
		fboGBuffer.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gBufferShader.use();
		gBufferShader.setProj(cam.proj);
		gBufferShader.setView(cam.view);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		if (Engine::wf)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		int i = 0;
		for (auto itr = entities.begin(); itr != entities.end(); ++itr)
		{

			if (i == 4)
			{
				glBindTextureUnit(0, 1);
				glBindTextureUnit(1, 1);
			}
			else
			{
				glBindTextureUnit(0, 3);
				glBindTextureUnit(1, 3);
			}
			glBindVertexArray(itr->first->vao);
			glBindBuffer(GL_ARRAY_BUFFER, itr->first->vbo);
			for (auto itr2 = itr->second.begin(); itr2 != itr->second.end(); ++itr2)
			{
				if (i == 0)
				{
					*itr2 = glm::rotate(*itr2, 0.05f, glm::fvec3(1.f, 1.f, 1.f));
				}
				glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(*itr2));
				glDrawArrays(GL_TRIANGLES, 0, itr->first->data.numVert);
			}
			++i;
		}

		if (Engine::wf)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		glBindVertexArray(0);

		// *********************************************************** G-BUFFER PASS *********************************************************** //

		glViewport(0, 0, shadRes.x*shadScale, shadRes.y*shadScale);

		fboLight.bind();
		fboLight.clear(GL_DEPTH_BUFFER_BIT);

		glm::fmat4 projj = glm::ortho(-shadRes.x / 2.f, shadRes.x / 2.f, -shadRes.y / 2.f, shadRes.y / 2.f, 100.f, 1000.f);
		//glm::fmat4 vview = glm::fmat4(0.937299, 0.219367, -0.2708289, 0, 0, 0.77707, 0.629414141, 0.0, 0.348525763, -0.589949369, 0.728347123, 0.0, -269.99057, -119.16075, -277.979919, 1.00);

		glm::fvec3 ppos(100, 100, 200);

		glm::fmat4 translate = glm::fmat4(1.0f);
		translate = glm::translate(translate, -ppos);

		glm::fmat4 vview = glm::lookAt(ppos, glm::fvec3(0), glm::fvec3(0, 1, 0));

		shadowShader.use();
		glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(projj));
		glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(vview));

		glEnable(GL_DEPTH_TEST);

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

		glBindVertexArray(0);

		// *********************************************************** SSAO PASS *********************************************************** //

		glViewport(0, 0, renderResolution.x * ssaoScale, renderResolution.y * ssaoScale);

		fboSSAO.bind();
		fboSSAO.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glCullFace(GL_FRONT);

		ssaoShader.use();
		glBindVertexArray(vaoQuad);
		fboGBuffer.textureAttachments[3].bind(0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// *********************************************************** SSAO PASS *********************************************************** //

		// *********************************************************** BLUR PASS *********************************************************** //

		fboSSAOBlur.bind();
		fboSSAOBlur.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		blurShader.use();

		glm::ivec2 axis(1, 0);
		glUniform2iv(glGetUniformLocation(blurShader(), "axis"), 1, glm::value_ptr(axis));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fboSSAO.textureAttachments[0].getGLID());
		glDrawArrays(GL_TRIANGLES, 0, 6);

		fboSSAO.bindDraw();
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		fboSSAOBlur.bindRead();
		glBlitFramebuffer(0, 0, float(viewport.width) * frameScale * ssaoScale, float(viewport.height) * frameScale * ssaoScale, 0, 0, float(viewport.width) * frameScale * ssaoScale, float(viewport.height) * frameScale * ssaoScale, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		fboSSAOBlur.bind();
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		axis = glm::ivec2(0, 1);
		glUniform2iv(glGetUniformLocation(blurShader(), "axis"), 1, glm::value_ptr(axis));
		glDrawArrays(GL_TRIANGLES, 0, 6);

		fboSSAO.bindDraw();
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		fboSSAOBlur.bindRead();
		glBlitFramebuffer(0, 0, float(viewport.width) * frameScale * ssaoScale, float(viewport.height) * frameScale * ssaoScale, 0, 0, float(viewport.width) * frameScale * ssaoScale, float(viewport.height) * frameScale * ssaoScale, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		// *********************************************************** BLUR PASS *********************************************************** //

		// *********************************************************** LIGHTING PASS *********************************************************** //

		glViewport(0, 0, renderResolution.x, renderResolution.y);
		fboScreen.bindDraw();

		//TODO: POST PROCESSING ON fboScreen

		glUseProgram(Engine::s());
		fboScreen.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		spotLights[0].position = cam.pos - glm::fvec3(0, 20, 0);
		spotLights[0].direction = cam.getDirectionVector() + glm::fvec3(0, -0.1, 0);

		auto shadLoc = glGetUniformLocation(Engine::s(), "lightMat");
		auto lightMat = projj * vview;
		glUniformMatrix4fv(shadLoc, 1, GL_FALSE, glm::value_ptr(lightMat));

		glUniformMatrix4fv(glGetUniformLocation(Engine::s(), "proj"), 1, GL_FALSE, glm::value_ptr(cam.proj));
		glUniformMatrix4fv(glGetUniformLocation(Engine::s(), "view"), 1, GL_FALSE, glm::value_ptr(cam.view));
		//glUniform3fv(glGetUniformLocation(Engine::s(), "spotLights[0].position"), 1, &spotLights[0].position[0]);
		//glUniform3fv(glGetUniformLocation(Engine::s(), "spotLights[0].direction"), 1, &spotLights[0].direction[0]);

		auto posloc = glGetUniformLocation(Engine::s(), "viewPos");
		glUniform3f(posloc, cam.pos.x, cam.pos.y, cam.pos.z);

		glDisable(GL_DEPTH_TEST);
		//glDisable(GL_CULL_FACE);

		glBindVertexArray(vaoQuad);

		fboGBuffer.textureAttachments[0].bind(0);
		fboGBuffer.textureAttachments[1].bind(1);
		fboGBuffer.textureAttachments[2].bind(2);
		fboSSAOBlur.textureAttachments[0].bind(3);
		fboLight.textureAttachments[0].bind(8);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		// *********************************************************** LIGHTING PASS *********************************************************** //

		// *********************************************************** SKYBOX PASS *********************************************************** //

		glCullFace(GL_BACK);

		fboDefault.bindDraw();
		fboScreen.bindRead();

		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glDrawBuffer(GL_BACK);
		glBlitFramebuffer(0, 0, renderResolution.x, renderResolution.y, 0, 0, viewport.width, viewport.height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		fboDefault.bindDraw();
		fboGBuffer.bindRead();

		//glReadBuffer(GL_COLOR_ATTACHMENT10);
		//glDrawBuffer(GL_FRONT);
		glBlitFramebuffer(0, 0, renderResolution.x, renderResolution.y, 0, 0, viewport.width, viewport.height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		glDrawBuffer(GL_BACK);

		glDepthFunc(GL_LEQUAL);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glm::mat4 view = cam.rotation;
		glm::mat4 projection = cam.proj;
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

		// *********************************************************** SKYBOX PASS *********************************************************** //

		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//fboDefault.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, glm::fvec4(0.5,0.5,0.5,0.5));
		//glDisable(GL_DEPTH_TEST);
		//Engine::t.shader->setProj(Engine::defaultOrthoCam.proj);
		//Engine::t.shader->setProj(cam.proj);
		//Engine::t.shader->setView(cam.view);
		//Engine::t.shader->setView(glm::fmat4(1));
		//Engine::t.shader->setModel(glm::fmat4(1));
		//Engine::t.draw();
		//glDisable(GL_BLEND);

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
	float ssaoScale;
	glm::ivec2 renderResolution;

	float ssaoPower;
	Window* window;
	int MSAALevel;
	GLuint SSAONoiseTex;


	Framebuffer fboSSAOBlur;
	Framebuffer fboSSAO;
	Framebuffer fboGBuffer;
	Framebuffer fboScreen;
	DefaultFramebuffer fboDefault;

	GLuint vaoQuad;
	GLuint vboQuad;

	Shader skyboxShader;
	GLuint skyboxTex;
	GLuint vaoSkybox, vboSkybox;

	GBufferShader gBufferShader;
	SAOShader ssaoShader;
	Shader blurShader;

	Shader shadowShader;
	Framebuffer fboLight;
	float shadScale;
	const glm::ivec2 shadRes = glm::ivec2(1000, 500);
};