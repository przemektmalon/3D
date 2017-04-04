#pragma once
#include <unordered_map>
#include "Mesh.h"
#include "Transform.h"
#include "SAOShader.h"
#include "GBufferShader.h"
#include "Framebuffer.h"
#include "Text.h"
#include "SSBO.h"
#include <chrono>
#include <time.h>
#include "Lights.h"
#include "ComputeShader.h"

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

const GLfloat quadVertices[] = {
	-1.0f,  1.0f,  0.0f, 1.0f,//TL
	1.0f,  1.0f,  1.0f, 1.0f,//TR
	1.0f, -1.0f,  1.0f, 0.0f,//BR

	1.0f, -1.0f,  1.0f, 0.0f,//BR
	-1.0f, -1.0f,  0.0f, 0.0f,//BL
	-1.0f,  1.0f,  0.0f, 1.0f//TL
};

GLfloat quadVerticesViewRays[] = {
	-1.0f,  1.0f,  0.0f, 1.0f, 999.f, 999.f,
	1.0f,  1.0f,  1.0f, 1.0f, 999.f, 999.f,
	1.0f, -1.0f,  1.0f, 0.0f, 999.f, 999.f,

	1.0f, -1.0f,  1.0f, 0.0f, 999.f, 999.f,
	-1.0f, -1.0f,  0.0f, 0.0f, 999.f, 999.f,
	-1.0f,  1.0f,  0.0f, 1.0f, 999.f, 999.f,
};



class MasterRenderer
{
public:
	MasterRenderer() : ssaoPower(1.f), ssaoScale(1.f), shadScale(1.f)
	{

	}
	~MasterRenderer() {}

	inline void initialiseScreenQuad()
	{
		glGenVertexArrays(1, &vaoQuad);
		glGenBuffers(1, &vboQuad);

		glBindBuffer(GL_ARRAY_BUFFER, vboQuad);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

		blurShader.load("res/shader/bilatBlur", "res/shader/bilatBlur"); blurShader.use();
		auto locc = glGetUniformLocation(blurShader(), "source");
		glUniform1i(locc, 0);

		blurShader.use();

		glBindVertexArray(vaoQuad);
		glBindBuffer(GL_ARRAY_BUFFER, vboQuad);

		GLint posAttrib = glGetAttribLocation(blurShader(), "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

		GLint texAttrib = glGetAttribLocation(blurShader(), "texCoord");
		glEnableVertexAttribArray(texAttrib);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));



		ssaoShader.use();



		posAttrib = glGetAttribLocation(ssaoShader(), "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

		texAttrib = glGetAttribLocation(ssaoShader(), "texCoord");
		glEnableVertexAttribArray(texAttrib);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

		glGenVertexArrays(1, &vaoQuadViewRays);
		glGenBuffers(1, &vboQuadViewRays);

		glBindBuffer(GL_ARRAY_BUFFER, vboQuadViewRays);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerticesViewRays), quadVerticesViewRays, GL_STATIC_DRAW);

		Engine::s.use();

		glBindVertexArray(vaoQuadViewRays);
		glBindBuffer(GL_ARRAY_BUFFER, vboQuadViewRays);

		posAttrib = glGetAttribLocation(Engine::s(), "position");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);

		texAttrib = glGetAttribLocation(Engine::s(), "texCoord");
		glEnableVertexAttribArray(texAttrib);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

		auto viewAttrib = glGetAttribLocation(Engine::s(), "viewRay");
		glEnableVertexAttribArray(viewAttrib);
		glVertexAttribPointer(viewAttrib, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(4 * sizeof(GLfloat)));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		Engine::testShader.use();

		glBindVertexArray(vaoQuadViewRays);
		glBindBuffer(GL_ARRAY_BUFFER, vboQuadViewRays);

		posAttrib = glGetAttribLocation(Engine::testShader(), "p");
		glEnableVertexAttribArray(posAttrib);
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);

		texAttrib = glGetAttribLocation(Engine::testShader(), "t");
		glEnableVertexAttribArray(texAttrib);
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

		viewAttrib = glGetAttribLocation(Engine::testShader(), "v");
		glEnableVertexAttribArray(viewAttrib);
		glVertexAttribPointer(viewAttrib, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(4 * sizeof(GLfloat)));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//STANDARD SHADER TODO: MOVE TO SPECIFIC CLASS

		Engine::s.use();

		auto c = glGetUniformLocation(Engine::s(), "gDepth");
		glUniform1i(c, 0);
		//c = glGetUniformLocation(Engine::s(), "gDepthLin");
		//glUniform1i(c, 4);
		auto cc = glGetUniformLocation(Engine::s(), "gNormal");
		glUniform1i(cc, 1);
		auto ccc = glGetUniformLocation(Engine::s(), "gAlbedoSpec");
		glUniform1i(ccc, 2);
		glUniform1i(glGetUniformLocation(Engine::s(), "ssaoTex"), 3);
		glUniform1i(glGetUniformLocation(Engine::s(), "shadow"), 8);

		Engine::s.stop();
	}

	inline void initialiseGBuffer()
	{
		//glm::fvec4 projInfo(-2.f / (viewport.width * cam.proj[0][0]), -2.f / (viewport.height*cam.proj[1][1]), (1.f - cam.proj[0][2]) / cam.proj[0][0], (1.f + cam.proj[1][2]) / cam.proj[1][1]);
		//glUniform4fv(glGetUniformLocation(ssaoShader(), "projInfo"), 1, glm::value_ptr(projInfo));

		fboGBuffer.setResolution(glm::fvec2(window->getSize()) * frameScale);
		fboGBuffer.attachTexture(GL_RG16F, GL_RG, GL_HALF_FLOAT, GL_COLOR_ATTACHMENT0);//NORMAL
		fboGBuffer.attachTexture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT1);//ALBEDO_SPEC
		fboGBuffer.attachTexture(GL_DEPTH_COMPONENT32F_NV, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
		fboGBuffer.attachTexture(GL_R32F, GL_RED, GL_FLOAT, GL_COLOR_ATTACHMENT2);

		fboGBuffer.checkStatus();

		GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);

		fboSSAO.setResolution(glm::fvec2(window->getSize()) * frameScale);
		fboSSAO.attachTexture(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0, glm::fvec2(ssaoScale));
		fboSSAO.checkStatus();

		fboSSAOBlur.setResolution(glm::fvec2(window->getSize()) * frameScale);
		fboSSAOBlur.attachTexture(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0, glm::fvec2(ssaoScale));
		fboSSAOBlur.checkStatus();


	}

	inline void initialiseScreenFramebuffer()
	{
		fboScreen.setResolution(glm::fvec2(window->getSize()) * frameScale);
		fboScreen.attachTexture(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0);
		fboScreen.checkStatus();
	}

	inline void initialiseSkybox()
	{
		skyboxShader.load("res/shader/skybox", "res/shader/skybox");
		skyboxShader.use();

		glGenVertexArrays(1, &vaoSkybox);
		glBindVertexArray(vaoSkybox);

		glGenBuffers(1, &vboSkybox);
		glBindBuffer(GL_ARRAY_BUFFER, vboSkybox);

		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

		glUniform1i(glGetUniformLocation(skyboxShader(), "skybox"), 5);

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

		//glActiveTexture(GL_TEXTURE5);
		glGenTextures(1, &skyboxTex);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
		for (GLuint i = 0; i < 6; i++)
		{
			image = SOIL_load_image(faces[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			//glGenerateMipmap(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			SOIL_free_image_data(image);
		}
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		skyboxShader.stop();
	}

	inline void initialiseLights()
	{
		const int nr = 4000;
		for (int i = 0; i < nr; ++i)
		{
			PointLightData light;
			light.position = glm::fvec3(float(Engine::rand() % 10000) - 5000, float(Engine::rand() % 50), float(Engine::rand() % 10000) - 5000);
			light.colour = glm::fvec3((Engine::rand() % 70) + 200, (Engine::rand() % 70) + 200, (Engine::rand() % 70) + 200);
			light.colour = glm::normalize(light.colour); light.colour *= glm::fvec3(1.2f);
			light.linear = 0.0000f;
			light.quadratic = 0.0003f;
			light.updateRadius();
			lightManager.addPointLight(light);
		}

		//TODO: LAST NULL LIGHT FOR TILE CULLING OVERRUN

		lightManager.updateAllPointLights();

		const int nr2 = 4000;
		for (int i = 0; i < nr2; ++i)
		{
			SpotLightData l;
			l.position = glm::fvec3(float(Engine::rand() % 10000) - 5000, float(Engine::rand() % 50), float(Engine::rand() % 10000) - 5000);
			auto cc = Engine::rand() % 3;
			switch (cc)
			{
			case(0):
				l.colour = glm::fvec3(10, 0, 0);
				break;
			case(1):
				l.colour = glm::fvec3(0, 10, 0);
				break;
			case(2):
				l.colour = glm::fvec3(0, 0, 10);
				break;
			}
			l.direction = glm::fvec3(0, -1.0, 0.1);
			l.innerSpread = glm::cos(PI / 4.f);
			l.outerSpread = glm::cos(PI / 4.f);
			l.linear = 0.0001;
			l.quadratic = 0.0001;
			l.updateRadius();
			lightManager.addSpotLight(l);
		}

		lightManager.updateAllSpotLights();

		Engine::s.use();

		//lights.bufferData(sizeof(PointLightData) * pointLights.size(), pointLights.data(), GL_DYNAMIC_DRAW);
		//lights.bindBase(2);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		/*Engine::s.use();

		SpotLight spot = SpotLight(glm::fvec3(200, 200, 200), glm::fvec3(-PI/4 - 0.3, -0.2, 0.f), glm::fvec3(0.f, 0.f, 5.f), 10, 30, 0.00001, 0.00003);
		SpotLight spot2 = SpotLight(glm::fvec3(150, 200, 150), glm::fvec3(-PI/4, -0.2, 0.f), glm::fvec3(0.f, 5.f, 0.f), 10, 30, 0.00001, 0.00003);
		SpotLight spot3 = SpotLight(glm::fvec3(150, 200, 250), glm::fvec3(-PI/4, -0.2, 0.f), glm::fvec3(5.f, 0.f, 0.f), 10, 30, 0.00001, 0.00003);

		spotLights.push_back(spot);
		spotLights.push_back(spot2);
		spotLights.push_back(spot3);*/

		/*for (int i = 0; i < spotLights.size(); ++i)
		{
		auto locc = glGetUniformLocation(Engine::s(), ("spotLights[" + std::to_string(i) + "].posRad").c_str());
		glUniform4fv(locc, 1, &spotLights[i].posRad[0]);
		glUniform4fv(glGetUniformLocation(Engine::s(), ("spotLights[" + std::to_string(i) + "].dirInner").c_str()), 1, &spotLights[i].dirInner[0]);
		glUniform4fv(glGetUniformLocation(Engine::s(), ("spotLights[" + std::to_string(i) + "].colOuter").c_str()), 1, &spotLights[i].colOuter[0]);
		glUniform4fv(glGetUniformLocation(Engine::s(), ("spotLights[" + std::to_string(i) + "].linQuad").c_str()), 1, &spotLights[i].linQuad[0]);
		}*/

		glm::fvec3 ddir(-2, -2, -4);
		ddir = glm::normalize(ddir);

		DirectLightData dir = DirectLightData(ddir, glm::fvec3(0.1f, 0.1f, 0.125f));

		auto locc = glGetUniformLocation(Engine::s(), "directLights[0].direction");
		glUniform3fv(locc, 1, &dir.direction[0]);
		glUniform3fv(glGetUniformLocation(Engine::s(), "directLights[0].colour"), 1, &dir.colour[0]);

		shadowShader.load("res/shader/shadowPass", "res/shader/shadowPass"); shadowShader.use();

		auto posAttrib = glGetAttribLocation(shadowShader(), "p");
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(posAttrib);

		fboLight.setResolution(glm::fvec2(shadRes) * frameScale * shadScale);
		//fboLight.attachTexture(GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0, glm::fvec2(ssaoScale));
		fboLight.attachTexture(GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
		//fboLight.attachTexture(GL_R32F, GL_RED, GL_FLOAT, GL_COLOR_ATTACHMENT0);
		//GLuint attachments[1] = { GL_COLOR_ATTACHMENT0 };
		//glDrawBuffers(1, attachments);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		fboLight.checkStatus();
	}

	void initialiseRenderer(int Msaalev, Window* pwin, Camera& cam)
	{
		window = pwin;
		MSAALevel = Msaalev;
		viewport.bot = 0; viewport.left = 0; viewport.width = window->getSizeX(); viewport.height = window->getSizeY();
		frameScale = 1.f;
		renderResolution.x = viewport.width * frameScale;
		renderResolution.y = viewport.height * frameScale;

		compShad.load("res/shader/tileCull");

		setActiveCam(cam);
		initialiseScreenQuad();
		initialiseSkybox();
		initialiseLights();
		initialiseGBuffer();
		initialiseScreenFramebuffer();

		//int numTilesX = window->getSizeX() / 16;
		//int numTilesY = window->getSizeY() / 16;
		//tiles.bufferData(sizeof(float) * 2 * numTilesX * numTilesY, NULL, GL_STREAM_DRAW);
		//tiles.bindBase(3);

		fboGBuffer.setClearDepth(0.f);
		fboLight.setClearDepth(0.f);
		glDepthRangedNV(1.f, -1.f);
		th.createFromStream(GL_RGBA32F, renderResolution.x, renderResolution.y, GL_RGBA, GL_FLOAT, NULL);
		glBindImageTexture(0, th.getGLID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);



		//lights.bufferData(sizeof(PointLight) * pointLights.size(), pointLights.data(), GL_STATIC_DRAW);
		//lights.bindBase(2);
	}

	void setActiveCam(Camera& cam)
	{

		//TL 4  5
		//TR 10 11
		//BR 16 17

		//BR 22 23
		//BL 28 29
		//TL 34 35
		ssaoShader.setProj(cam.proj, glm::ivec2(viewport.width, viewport.height));
		quadVerticesViewRays[4] = cam.viewRays2[3].x;
		quadVerticesViewRays[5] = cam.viewRays2[3].y;

		quadVerticesViewRays[10] = cam.viewRays2[2].x;
		quadVerticesViewRays[11] = cam.viewRays2[2].y;

		quadVerticesViewRays[16] = cam.viewRays2[1].x;
		quadVerticesViewRays[17] = cam.viewRays2[1].y;

		quadVerticesViewRays[22] = cam.viewRays2[1].x;
		quadVerticesViewRays[23] = cam.viewRays2[1].y;

		quadVerticesViewRays[28] = cam.viewRays2[0].x;
		quadVerticesViewRays[29] = cam.viewRays2[0].y;

		quadVerticesViewRays[34] = cam.viewRays2[3].x;
		quadVerticesViewRays[35] = cam.viewRays2[3].y;

		compShad.use();

		//auto loc = glGetUniformLocation(compShad(), "viewRays");
		//glUniform4fv(loc, 1, &cam.viewRaysDat[0]);
	}

	void render(Camera& cam)
	{
		// *********************************************************** G-BUFFER PASS *********************************************************** //

		glViewport(0, 0, renderResolution.x, renderResolution.y);

		fboGBuffer.bind();
		fboGBuffer.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//fboDefault.bind();
		//fboDefault.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gBufferShader.use();
		gBufferShader.setProj(cam.proj);
		gBufferShader.setView(cam.view);

		glDepthRangedNV(-1.f, 1.f);
		glClearDepth(1.f);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glDepthFunc(GL_LESS);

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
					//*itr2 = glm::rotate(*itr2, 0.05f, glm::fvec3(1.f, 1.f, 1.f));
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

		//fboDefault.bind();
		//fboDefault.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		fboLight.bind();
		fboLight.clear(GL_DEPTH_BUFFER_BIT);

		//glm::fmat4 projj = glm::ortho(-shadRes.x/2.f, shadRes.x/2.f, -shadRes.y/2.f, shadRes.y/2.f, 10.f, 1000.f);
		glm::fmat4 projj = glm::perspective(float(PI) / 8.f, 1.f, 10.f, 1000.f);
		//glm::fmat4 vview = glm::fmat4(0.937299, 0.219367, -0.2708289, 0, 0, 0.77707, 0.629414141, 0.0, 0.348525763, -0.589949369, 0.728347123, 0.0, -269.99057, -119.16075, -277.979919, 1.00);

		/*if (Engine::movingLight)
		{
		auto l = lightManager.getSpotLight(0);
		l->position = cam.pos - glm::fvec3(0, 10, 0);
		l->direction = cam.getDirectionVector();
		lightManager.updateSpotLight(0);
		lightView = glm::translate(cam.view, glm::fvec3(0, 10, 0));
		}*/

		//glm::fmat4 vview = glm::lookAt(ppos, LPOS - glm::fvec3(0, 0, 1), glm::fvec3(0, 1, 0));
		glm::fmat4 vview = lightView;

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

		//fboDefault.bind();
		//fboDefault.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		fboSSAO.bind();
		fboSSAO.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glCullFace(GL_FRONT);

		ssaoShader.use();

		glBindVertexArray(vaoQuad);
		fboGBuffer.textureAttachments[2].bind(0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//fboGBuffer.textureAttachments[0].saveToFile("AAAAA",true);

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

		//fboSSAOBlur.textureAttachments[0].saveToFile("BLURR");

		// *********************************************************** BLUR PASS *********************************************************** //

		// *********************************************************** LIGHTING PASS *********************************************************** //

		/*glViewport(0, 0, renderResolution.x, renderResolution.y);
		fboScreen.bindDraw();

		//TODO: POST PROCESSING ON fboScreen

		glUseProgram(Engine::s());
		fboScreen.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto shadLoc = glGetUniformLocation(Engine::s(), "lightMat");
		auto lightMat = projj * vview;
		glUniformMatrix4fv(shadLoc, 1, GL_FALSE, glm::value_ptr(lightMat));

		glUniformMatrix4fv(glGetUniformLocation(Engine::s(), "proj"), 1, GL_FALSE, glm::value_ptr(cam.proj));
		glUniformMatrix4fv(glGetUniformLocation(Engine::s(), "view"), 1, GL_FALSE, glm::value_ptr(cam.view));

		auto posloc = glGetUniformLocation(Engine::s(), "viewPos");
		glUniform3f(posloc, cam.pos.x, cam.pos.y, cam.pos.z);

		glDisable(GL_DEPTH_TEST);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(vaoQuadViewRays);
		glBindBuffer(GL_ARRAY_BUFFER, vboQuadViewRays);

		fboGBuffer.textureAttachments[2].bind(0);
		fboGBuffer.textureAttachments[0].bind(1);
		fboGBuffer.textureAttachments[1].bind(2);
		fboSSAOBlur.textureAttachments[0].bind(3);
		fboGBuffer.textureAttachments[3].bind(4);
		fboLight.textureAttachments[0].bind(8);

		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo);
		//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo);

		//lights.bindBase(2);

		lightManager.pointLightsBuffer.bindBase(2);
		lightManager.spotLightsBuffer.bindBase(3);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		lightManager.pointLightsBuffer.unbind();
		lightManager.spotLightsBuffer.unbind();

		//lights.unbind();

		glBindVertexArray(0);*/

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

		glDepthFunc(GL_GEQUAL);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glm::mat4 view = cam.rotation;
		glm::mat4 projection = cam.infProj;
		glUseProgram(skyboxShader());
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader(), "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(vaoSkybox);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
		glUniform1i(glGetUniformLocation(skyboxShader(), "skybox"), 5);

		//glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);

		glDepthFunc(GL_GREATER);


		compShad.use();

		auto loc = glGetUniformLocation(compShad(), "proj");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(cam.proj));
		loc = glGetUniformLocation(compShad(), "view");
		glUniformMatrix4fv(loc, 1, GL_TRUE, glm::value_ptr(cam.view));
		auto posloc = glGetUniformLocation(compShad(), "viewPos");
		glUniform3f(posloc, cam.pos.x, cam.pos.y, cam.pos.z);
		loc = glGetUniformLocation(compShad(), "pointLightCount");
		glUniform1ui(loc, lightManager.pointLights.size());
		loc = glGetUniformLocation(compShad(), "spotLightCount");
		glUniform1ui(loc, lightManager.spotLights.size());

		glBindTextureUnit(2, th.getGLID());
		glBindImageTexture(2, th.getGLID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		glBindTextureUnit(3, fboGBuffer.textureAttachments[0].getGLID());
		glBindImageTexture(3, fboGBuffer.textureAttachments[0].getGLID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

		fboGBuffer.textureAttachments[0].bindImage(3, GL_READ_ONLY);
		fboGBuffer.textureAttachments[1].bindImage(4, GL_READ_ONLY);
		//fboGBuffer.textureAttachments[2].bindImage(5, GL_READ_ONLY);
		fboGBuffer.textureAttachments[2].bind(5);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);


		//cam.calculateViewRaysDat();

		glm::fvec4 vrays;
		vrays.x = cam.viewRays2[2].x;
		vrays.y = cam.viewRays2[2].y;
		vrays.z = cam.viewRays2[0].x;
		vrays.w = cam.viewRays2[0].y;

		loc = glGetUniformLocation(compShad(), "viewRays");
		glUniform4fv(loc, 1, &vrays.x);


		//glDispatchComputeGroupSizeARB(1280/16, 720/16, 1, 16, 16, 1);
		lightManager.pointLightsBuffer.bindBase(0);
		lightManager.spotLightsBuffer.bindBase(1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		glDispatchCompute(renderResolution.x / 16, renderResolution.y / 16, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		lightManager.pointLightsBuffer.unbind();
		lightManager.spotLightsBuffer.unbind();
		glFlush();

		fboDefault.bind();
		fboDefault.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		glBindVertexArray(vaoQuadViewRays);
		Engine::testShader.use();

		loc = glGetUniformLocation(Engine::testShader(), "tex");
		glUniform1i(loc, 0);
		auto locc = glGetUniformLocation(Engine::testShader(), "proj");
		glUniformMatrix4fv(locc, 1, GL_FALSE, glm::value_ptr(cam.proj));
		auto locc2 = glGetUniformLocation(Engine::testShader(), "view");
		glUniformMatrix4fv(locc2, 1, GL_FALSE, glm::value_ptr(cam.view));
		auto loccc = glGetUniformLocation(Engine::testShader(), "camPos");
		glUniform3fv(loccc, 1, &cam.pos[0]);

		th.bind(0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		/*Engine::testShader.use();

		glBindVertexArray(entities.begin()->first->vao);
		glDrawArrays(GL_TRIANGLES, 0, entities.begin()->first->data.numVert);

		glm::vec2 tileScale = glm::vec2(1280, 720) * (1.f / float(2 * 16));
		glm::vec2 tileBias = tileScale - glm::vec2(40, 22);

		auto proj = cam.proj;

		glm::vec4 c1 = glm::vec4(-proj[0][0] * tileScale.x, 0.f, tileBias.x, 0.f);
		glm::vec4 c2 = glm::vec4(0.f, -proj[1][1] * tileScale.y, tileBias.y, 0.f);
		glm::vec4 c4 = glm::vec4(0.f, 0.f, -1.f, 0.f);

		glm::vec4 planes[6];

		//float minDepthZ = float(minZf / float(0xFFFFFFFF));
		//float maxDepthZ = float(maxZf / float(0xFFFFFFFF));
		float minDepthZ = 0.5f;
		float maxDepthZ = 1.f;

		//planes[0] = c4 - c1;
		//planes[1] = c4 + c1;
		//planes[2] = c4 - c2;
		//planes[3] = c4 + c2;
		planes[0] = proj[3] - proj[0];//Right
		planes[1] = proj[3] + proj[0];//Left
		planes[2] = proj[3] - proj[1];//Top
		planes[3] = proj[3] + proj[1];//Bottom
		planes[4] = glm::vec4(0.f, 0.f, 1.f, -minDepthZ);
		planes[5] = glm::vec4(0.f, 0.f, -1.f, maxDepthZ);

		planes[0] *= 1.f / glm::length(glm::vec3(planes[0]));
		planes[1] *= 1.f / glm::length(glm::vec3(planes[1]));
		planes[2] *= 1.f / glm::length(glm::vec3(planes[2]));
		planes[3] *= 1.f / glm::length(glm::vec3(planes[3]));

		glm::fvec4 lightPos(0, 0, 0, 1.f);
		lightPos = cam.view * lightPos;
		float rad = 1.f;
		bool inFrustum = true;
		for (int i = 3; i >= 0 && inFrustum; i--)
		{
		float dist = glm::dot(planes[i], lightPos);
		inFrustum = bool(-rad <= dist);
		}

		std::cout << inFrustum << std::endl;*/

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
		//glDisable(GL_BLEND);*/

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
	std::vector<PointLightData> pointLights;
	std::vector<SpotLightData> spotLights;
	std::vector<DirectLightData> directLights;

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

	GLuint vaoQuadViewRays;
	GLuint vboQuadViewRays;

	Shader skyboxShader;
	GLuint skyboxTex;
	GLuint vaoSkybox, vboSkybox;

	GBufferShader gBufferShader;
	SAOShader ssaoShader;
	Shader blurShader;

	Shader shadowShader;
	Framebuffer fboLight;
	float shadScale;
	const glm::ivec2 shadRes = glm::ivec2(1000, 1000);

	LightManager lightManager;
	SSBO tiles;

	ComputeShader compShad;
	GLTexture2D th;
	glm::fmat4 lightView;
};