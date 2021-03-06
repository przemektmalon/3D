#include "Renderer.hpp"
#include "RendererData.hpp"
#include "Engine.hpp"
#include "Time.hpp"
#include "QPC.hpp"
#include "SOIL.h"
#include "Camera.hpp"
#include "UILabel.hpp"
#include "AssetManager.hpp"
#include "World.hpp"
#include "Text.hpp"
#include "GPUModelManager.hpp"
#include "Console.hpp"
#include "Rect.hpp"
#include "MathIncludes.hpp"

#include "UIRectangleShape.hpp"
#include "UIConsole.hpp"
#include "UIButton.hpp"

#include <limits>

#define NUM_POINT_LIGHTS 5
#define NUM_SPOT_LIGHTS 0

void Renderer::render()
{
	GL_PROFILE_START("gpuBuffer");

	world->updateDrawBuffer(); // For LOD

	for (int i = 0; i < lightManager.spotLights.size(); ++i)
	{
		auto light = lightManager.getSpotLight(i);

		light->setPosition(glm::fvec3(std::cos(Engine::programTime*0.2*(i + 1) + (i*PI*0.5))*50.f, 50.f, std::sin(Engine::programTime*0.2*(i + 1) + (i*PI*0.5))*50.f));
		light->setDirection(-glm::normalize(light->getPosition() + glm::fvec3(0.f, 20.f, 0.f)));
	}

	for (int i = 0; i < lightManager.pointLights.size(); ++i)
	{
		auto light = lightManager.getPointLight(i);

		glm::fvec3 position;

		position.y = 100.f + (10.f * std::sin(Engine::programTime * 0.4f));
		position.x = 40.f * std::sin(Engine::programTime * 0.4f + ((i + 1) * 2 * PI / NUM_POINT_LIGHTS));
		position.z = 40.f * std::cos(Engine::programTime * 0.4f + ((i + 1) * 2 * PI / NUM_POINT_LIGHTS));
		
		position.x *= (2.f + std::sin(Engine::programTime * 0.8f)) * 1.4f;
		position.z *= (2.f + std::sin(Engine::programTime * 0.8f)) * 1.4f;

		light->setPosition(position);
		light->setLinear(Engine::linear);
		light->setQuadratic(Engine::quad);
	}

	lightManager.updateAllPointLights();
	lightManager.updateAllSpotLights();
	lightManager.updateSunLight();

	GL_PROFILE_END("gpuBuffer");

	GL_PROFILE_THIS(gBufferPass(), "gBuffer");
	
	GL_PROFILE_THIS(shadowPass(), "shadow");

	GL_PROFILE_THIS(ssaoPass(), "ssao");

	GL_PROFILE_THIS(shadingPass(), "light");

	GL_PROFILE_START("screen");

	screenPass();

	lightManager.drawLightIcons();

	for (auto tag : world->tags)
	{
		tag.second.draw();
	}

	Engine::uiwm.drawUIWindows();

	//Engine::console.draw();

	window->swapBuffers();

	GL_PROFILE_END("screen");
}

void Renderer::gBufferPass()
{
	glViewport(0, 0, Engine::cfg.render.resolution.x, Engine::cfg.render.resolution.y);
	fboGBuffer.bind();

	glDepthRangedNV(-1.f, 1.f);

	fboGBuffer.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glm::ivec4 clearC(-1, -1, -1, -1);
	//glClearBufferiv(GL_COLOR, GL_COLOR_ATTACHMENT2, &clearC.x); // Clear IDs buffer

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glCullFace(GL_BACK);

	gBufferShaderTex.use();

	gBufferShaderTex.setView(activeCam->view);
	gBufferShaderTex.setCamPos(activeCam->pos);

	gBufferShaderTex.sendView();
	gBufferShaderTex.sendCamPos();

	gBufferShaderTex.sendUniforms();

	Engine::assets.modelManager.regularBatch.vao.bind();

	world->texHandleBuffer[Regular].bindBase(GL_SHADER_STORAGE_BUFFER, 3);
	world->instanceTransformsBuffer[Regular].bindBase(GL_SHADER_STORAGE_BUFFER, 4);
	world->drawIndirectBuffer[Regular].bind(GL_DRAW_INDIRECT_BUFFER);

	glMultiDrawArraysIndirect(GL_TRIANGLES, 0, world->modelInstances.size(), 0);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}

void Renderer::shadowPass()
{
	pointShadowPassShader.use();

	glDepthRange(0.f, 1.f);
	//glDepthRangedNV(-1.f, 1.f);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	for (auto itr = lightManager.pointLights.begin(); itr != lightManager.pointLights.end(); ++itr)
	{
		if (glm::length(itr->getPosition() - activeCam->pos) > itr->getFadeStart() + itr->getFadeLength())
			continue;

		glViewport(0, 0, itr->getShadowTexture()->getWidth(), itr->getShadowTexture()->getHeight());

		fboShadow.bind();
		fboShadow.attachForeignCubeTexture(itr->getShadowTexture(), GL_DEPTH_ATTACHMENT);

		glClear(GL_DEPTH_BUFFER_BIT);

		shadowMatrixBuffer.bufferSubData(0, sizeof(glm::fmat4) * 6, &itr->getProjView()[0][0]);
		shadowMatrixBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 2);

		pointShadowPassShader.setFarPlane(itr->getRadius());
		pointShadowPassShader.setLightPos(itr->getPosition());
		pointShadowPassShader.sendUniforms();

		Engine::assets.modelManager.shadowVAO.bind();
		world->drawIndirectBuffer[Regular].bind(GL_DRAW_INDIRECT_BUFFER);
		world->instanceTransformsBuffer[Regular].bindBase(GL_SHADER_STORAGE_BUFFER, 1);

		glMultiDrawArraysIndirect(GL_TRIANGLES, 0, world->modelInstances.size(), 0);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

		glBindVertexArray(0);
	}

	spotShadowPassShader.use();

	for (auto itr = lightManager.spotLights.begin(); itr != lightManager.spotLights.end(); ++itr)
	{
		if (glm::length(itr->getPosition() - activeCam->pos) > itr->getFadeStart() + itr->getFadeLength())
			continue;

		glViewport(0, 0, itr->getShadowTexture()->getWidth(), itr->getShadowTexture()->getHeight());

		fboShadow.bind();
		fboShadow.attachForeignTexture(itr->getShadowTexture(), GL_DEPTH_ATTACHMENT);

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		spotShadowPassShader.setProj(itr->getProjView());
		spotShadowPassShader.setView(glm::fmat4(1.f));
		spotShadowPassShader.sendUniforms();

		Engine::assets.modelManager.shadowVAO.bind();
		world->drawIndirectBuffer[Regular].bind(GL_DRAW_INDIRECT_BUFFER);
		world->instanceTransformsBuffer[Regular].bindBase(GL_SHADER_STORAGE_BUFFER, 1);

		glMultiDrawArraysIndirect(GL_TRIANGLES, 0, world->modelInstances.size(), 0);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

		glBindVertexArray(0);
	}

	/*float cascadeEnds[4];

	cascadeEnds[0] = activeCam->nearClip;
	cascadeEnds[1] = 1050.0f;
	cascadeEnds[2] = 3000.0f;
	cascadeEnds[3] = 5000.f;

	lightManager.sunLight.getData()->cascadeEnds[0] = cascadeEnds[0];
	lightManager.sunLight.getData()->cascadeEnds[1] = cascadeEnds[1];
	lightManager.sunLight.getData()->cascadeEnds[2] = cascadeEnds[2];
	lightManager.sunLight.getData()->cascadeEnds[3] = cascadeEnds[3];

	glm::fmat4 lightM = glm::translate(glm::fmat4(), glm::fvec3(lightManager.sunLight.getDirection() * -100.f)) * glm::lookAt(glm::fvec3(0, 0, 0), lightManager.sunLight.getDirection(), glm::fvec3(0, 1, 0));

	//lightM = activeCam->view;

	float tanHalfHFOV = tanf(glm::radians(activeCam->fov / 2.0f));
	float tanHalfVFOV = tanf(glm::radians((activeCam->fov * activeCam->aspect) / 2.0f));

	int maxRes[3] = { 1024, 512, 256 };

	for (int i = 0; i < 3; i++) {
		float xn = cascadeEnds[i] * tanHalfHFOV;
		float xf = cascadeEnds[i + 1] * tanHalfHFOV;
		float yn = cascadeEnds[i] * tanHalfVFOV;
		float yf = cascadeEnds[i + 1] * tanHalfVFOV;

		glm::fvec4 frustumCorners[8] = {
			// near face
			glm::fvec4(xn, yn, cascadeEnds[i], 1.0),
			glm::fvec4(-xn, yn, cascadeEnds[i], 1.0),
			glm::fvec4(xn, -yn, cascadeEnds[i], 1.0),
			glm::fvec4(-xn, -yn, cascadeEnds[i], 1.0),

			// far face
			glm::fvec4(xf, yf, cascadeEnds[i + 1], 1.0),
			glm::fvec4(-xf, yf, cascadeEnds[i + 1], 1.0),
			glm::fvec4(xf, -yf, cascadeEnds[i + 1], 1.0),
			glm::fvec4(-xf, -yf, cascadeEnds[i + 1], 1.0)
		};

		glm::fvec4 frustumCornersL[8];

		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::min();
		float minY = std::numeric_limits<float>::max();
		float maxY = std::numeric_limits<float>::min();
		float minZ = std::numeric_limits<float>::max();
		float maxZ = std::numeric_limits<float>::min();

		for (int j = 0; j < 8; j++) {

			// Transform the frustum coordinate from view to world space
			glm::fvec4 vW = activeCam->inverseView * frustumCorners[j];

			// Transform the frustum coordinate from world to light space
			frustumCornersL[j] = lightM * vW;

			minX = std::min(minX, frustumCornersL[j].x);
			maxX = std::max(maxX, frustumCornersL[j].x);
			minY = std::min(minY, frustumCornersL[j].y);
			maxY = std::max(maxY, frustumCornersL[j].y);
			minZ = std::min(minZ, frustumCornersL[j].z);
			maxZ = std::max(maxZ, frustumCornersL[j].z);
		}

		auto ortho = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
		glm::fmat4 pr = glm::ortho<float>(-1000, 1000, -1000, 1000, -1000, 1000) * glm::lookAt(glm::fvec3(0, 0, 0), lightManager.sunLight.getDirection(), glm::fvec3(0, 1, 0));
		lightManager.sunLight.setProjView(i, ortho);

		glm::ivec2 resolution;

		float width = maxX - minX;
		float height = maxY - minY;

		if (width > height)
		{
			float resFactor = width / maxRes[i];
			csmViewports[i].x = maxRes[i];
			csmViewports[i].y /= resFactor;
		}
		else
		{
			float resFactor = height / maxRes[i];
			csmViewports[i].y = maxRes[i];
			csmViewports[i].x /= resFactor;
		}

		glm::fmat4 cpr;

		if (i == 1)
		{
			tileCullShader.use();
			auto col = glGetUniformLocation(tileCullShader.getGLID(), "sunCol");
			auto dir = glGetUniformLocation(tileCullShader.getGLID(), "sunDir");
			auto handle = glGetUniformLocation(tileCullShader.getGLID(), "sunHandle");
			auto pv = glGetUniformLocation(tileCullShader.getGLID(), "sunPV");
			auto cas = glGetUniformLocation(tileCullShader.getGLID(), "sunCascadeEnd");

			glm::fmat4 cpr = glm::lookAt(glm::fvec3(-10, 100, -20), glm::fvec3(0, 0, 0), glm::fvec3(0, 1, 0));

			glUniform4f(col, 1, 0, 1, 1);
			glUniform4fv(dir, 1, &lightManager.sunLight.getDirection()[0]);
			glUniform2uiv(handle, 1, (GLuint*)&lightManager.sunLight.getData()->shadowHandles[1]);
			glUniformMatrix4fv(pv, 1, GL_FALSE, glm::value_ptr(cpr));
			glUniform1f(cas, cascadeEnds[2]);
		}

		glViewport(0, 0, int(width), int(height));

		fboShadow.bind();
		fboShadow.attachForeignTexture((GLTexture2D*)(lightManager.sunLight.getShadowTexture() + i), GL_DEPTH_ATTACHMENT);

		glClear(GL_DEPTH_BUFFER_BIT);

		spotShadowPassShader.use();

		spotShadowPassShader.setProj(glm::perspective<float>(3.1415/2.f,1.f, 1.f, 2000.f) * glm::translate(glm::fmat4(), glm::fvec3(-30,300,60)) * cpr);
		//spotShadowPassShader.setView(glm::lookAt(glm::fvec3(0, 0, 0), glm::fvec3(0.1, -1, 0.2), glm::fvec3(0, 1, 0)));
		spotShadowPassShader.setView(glm::fmat4(1.f));
		spotShadowPassShader.sendUniforms();

		Engine::assets.modelManager.shadowVAO.bind();
		world->drawIndirectBuffer[Regular].bind(GL_DRAW_INDIRECT_BUFFER);
		world->instanceTransformsBuffer[Regular].bindBase(GL_SHADER_STORAGE_BUFFER, 1);

		glMultiDrawArraysIndirect(GL_TRIANGLES, 0, world->modelInstances.size(), 0);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

		glFinish();

		u8* pix = new u8[sizeof(float) * int(width) * int(height)];

		glBindTexture(GL_TEXTURE_2D, lightManager.sunLight.getShadowTexture()[i].getGLID());
		glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, GL_FLOAT, pix);

		delete[] pix;

		glBindVertexArray(0);
	}
	lightManager.updateSunLight();*/
}

void Renderer::ssaoPass()
{
	Engine::cfg.render.frameScale = 1.f;
	glViewport(0, 0, Engine::cfg.render.resolution.x * Engine::cfg.render.frameScale, Engine::cfg.render.resolution.y * Engine::cfg.render.frameScale);

	fboSSAO.bind();
	fboSSAO.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	ssaoShader.use();
	ssaoShader.sendUniforms();

	//glBindVertexArray(vaoQuad);
	vaoQuad.bind();
	fboGBuffer.textureAttachments[3].bind(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_BLEND);

	fboSSAOBlur.bind();
	fboSSAOBlur.clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	bilatBlurShader.use();

	glm::ivec2 axis(1, 0);
	bilatBlurShader.setAxis(axis);
	bilatBlurShader.sendUniforms();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fboSSAO.textureAttachments[0].getGLID());
	glDrawArrays(GL_TRIANGLES, 0, 6);

	fboSSAO.bindDraw();
	fboSSAOBlur.bindRead();
	glBlitFramebuffer(0, 0, Engine::cfg.render.resolution.x * Engine::cfg.render.frameScale, Engine::cfg.render.resolution.y * Engine::cfg.render.frameScale, 0, 0, Engine::cfg.render.resolution.x * Engine::cfg.render.frameScale, Engine::cfg.render.resolution.y * Engine::cfg.render.frameScale, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	fboSSAOBlur.bind();

	axis = glm::ivec2(0, 1);
	bilatBlurShader.setAxis(axis);
	bilatBlurShader.sendUniforms();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	fboSSAO.bindDraw();
	fboSSAOBlur.bindRead();
	glBlitFramebuffer(0, 0, Engine::cfg.render.resolution.x * Engine::cfg.render.frameScale, Engine::cfg.render.resolution.y * Engine::cfg.render.frameScale, 0, 0, Engine::cfg.render.resolution.x * Engine::cfg.render.frameScale, Engine::cfg.render.resolution.y * Engine::cfg.render.frameScale, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void Renderer::shadingPass()
{
	tileCullShader.use();

	glBindTextureUnit(2, lightPassTex.getGLID());
	glBindImageTexture(2, lightPassTex.getGLID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glBindTextureUnit(3, fboGBuffer.textureAttachments[0].getGLID());
	glBindImageTexture(3, fboGBuffer.textureAttachments[0].getGLID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

	fboGBuffer.textureAttachments[0].bindImage(3, GL_READ_ONLY);
	fboGBuffer.textureAttachments[1].bindImage(4, GL_READ_ONLY);
	fboGBuffer.textureAttachments[2].bindImage(7, GL_READ_ONLY);
	fboGBuffer.textureAttachments[3].bind(5);
	fboSSAO.textureAttachments[0].bindImage(6, GL_READ_ONLY);
	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_CUBE_MAP, world->skybox.getGLID());
	//world->skybox.bind(15);

	glm::fvec4 vrays;
	vrays.x = activeCam->viewRays2[2].x;
	vrays.y = activeCam->viewRays2[2].y;
	vrays.z = activeCam->viewRays2[0].x;
	vrays.w = activeCam->viewRays2[0].y;

	tileCullShader.setViewRays(vrays);
	tileCullShader.setView(activeCam->view);
	tileCullShader.setViewPos(activeCam->pos);

	tileCullShader.sendViewRays();
	tileCullShader.sendView();
	tileCullShader.sendViewPos();

	tileCullShader.sendUniforms();

	lightManager.pointLightsBuffer.bindBase(0);
	lightManager.spotLightsBuffer.bindBase(1);
	lightManager.sunLightBuffer.bindBase(8);
	glFinish();
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glDispatchCompute(std::ceil(Engine::cfg.render.resolution.x / 16.f), std::ceil(float(Engine::cfg.render.resolution.y) / 16.f), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	lightManager.pointLightsBuffer.unbind();
	lightManager.spotLightsBuffer.unbind();
	lightManager.sunLightBuffer.unbind();
}

void Renderer::screenPass()
{
	DefaultFBO::bind();
	DefaultFBO::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	vaoQuadViewRays.bind();
	auto program = shaderStore.getShader("test");
	program->use();

	glUniform1i(0, 2);
	lightPassTex.bind(2);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	if (Engine::cfg.render.drawWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		auto wireShader = shaderStore.getShader("wireframe");
		wireShader->use();
		wireShader->setUniform("proj", &activeCam->proj);
		wireShader->setUniform("view", &activeCam->view);
		float col = std::sin(Engine::programTime*10.f) + 1.f; col *= 0.05; col += 0.05f;
		glm::fvec4 col4(1.f, 1.f, 1.f, col);
		wireShader->setUniform("wireColour", &col4);
		wireShader->sendUniforms();

		Engine::assets.modelManager.shadowVAO.bind();
		world->drawIndirectBuffer[Regular].bind(GL_DRAW_INDIRECT_BUFFER);
		world->instanceTransformsBuffer[Regular].bindBase(GL_SHADER_STORAGE_BUFFER, 1);

		glMultiDrawArraysIndirect(GL_TRIANGLES, 0, world->modelInstances.size(), 0);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (Engine::cfg.render.drawAABB)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		auto wireShader = shaderStore.getShader("wireframe");
		wireShader->use();
		wireShader->setUniform("proj", &activeCam->proj);
		wireShader->setUniform("view", &activeCam->view);
		float col = std::sin(Engine::programTime*5.f) + 1.f; col *= 0.5; col += 0.5f;
		glm::fvec4 col4(0.f, 0.f, 1.f, col);
		wireShader->setUniform("wireColour", &col4);
		wireShader->sendUniforms();

		for (auto& instance : world->modelInstances)
		{
			instance.second.physicsObject->updateGLAABB();
		}

		Engine::assets.modelManager.aabbBatch.vao.bind();
		world->aabbIndirectBuffer.bind(GL_DRAW_INDIRECT_BUFFER);
		world->aabbTransformsBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 1);

		glMultiDrawArraysIndirect(GL_QUADS, 0, world->modelInstances.size(), 0);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void Renderer::initialiseRenderer(Window * pwin, Camera & cam)
{
	window = pwin;
	viewport.top = 0; viewport.left = 0; viewport.width = window->getSizeX(); viewport.height = window->getSizeY();
	Engine::cfg.render.frameScale = 1.f;

	initialiseSamplers();

	setActiveCam(cam);

	initialiseScreenQuad();
	initialiseLights();

	if (Engine::cfg.render.resolution != glm::ivec2(0, 0))
	{
		initialiseFramebuffers();
		lightPassTex.createFromStream(GL_RGBA32F, Engine::cfg.render.resolution.x, Engine::cfg.render.resolution.y, GL_RGBA, GL_FLOAT, NULL);
	}

	shadowMatrixBuffer.bufferData(sizeof(glm::fmat4) * 6, nullptr, GL_STREAM_DRAW);
	fboGBuffer.setClearDepth(0.f);
}

inline void Renderer::initialiseGBuffer()
{
	fboGBuffer.create();
	fboGBuffer.setResolution(Engine::cfg.render.resolution);
	fboGBuffer.attachTexture(GL_RG16F, GL_RG, GL_HALF_FLOAT, GL_COLOR_ATTACHMENT0);							// NORMAL
	fboGBuffer.attachTexture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT1);					// ALBEDO_SPEC
	fboGBuffer.attachTexture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT2);					// PBR
	fboGBuffer.attachTexture(GL_DEPTH_COMPONENT32F_NV, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);	// DEPTH
	
	fboGBuffer.checkStatus();

	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
}

inline void Renderer::initialiseSSAOBuffer()
{
	fboSSAO.create();
	fboSSAO.setResolution(Engine::cfg.render.resolution);
	fboSSAO.attachTexture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0, glm::fvec2(Engine::cfg.render.frameScale));
	fboSSAO.checkStatus();

	fboSSAOBlur.create();
	fboSSAOBlur.setResolution(Engine::cfg.render.resolution);
	fboSSAOBlur.attachTexture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0, glm::fvec2(Engine::cfg.render.frameScale));
	fboSSAOBlur.checkStatus();
}

inline void Renderer::initialiseScreenFramebuffer()
{
	fboScreen.create();
	fboScreen.setResolution(Engine::cfg.render.resolution);
	fboScreen.attachTexture(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0);
	fboScreen.checkStatus();
}

inline void Renderer::initialiseLights()
{
	shadowMatrixBuffer.create();

	const int nr = NUM_POINT_LIGHTS;
	for (int i = 0; i < nr; ++i)
	{
		auto& add = lightManager.addPointLight();
		auto cc = Engine::rand() % 3;
		glm::fvec3 col(0.f);
		switch (i%4)
		{
		case 0:
			col = glm::fvec3(1.f, 61.f / 255.f, 55.f / 255.f);
			break;
		case 1:
			col = glm::fvec3(232.f / 255.f, 197.f / 255.f, 50.f / 255.f);
			break;
		case 2:
			col = glm::fvec3(42.f / 255.f, 255.f / 255.f, 118.f / 255.f);
			break;
		case 3:
			col = glm::fvec3(38.f / 255.f, 45.f / 255.f, 232.f / 255.f);
			break;
		default:
			col = glm::fvec3(1.f, 1.f, 1.f);
			break;
		}
		col = glm::fvec3(1.f, 1.f, 1.f);
		add.setColour(col * 5.5f);
		add.setLinear(0.00001f);
		add.setQuadratic(0.005f);
		add.setPosition(glm::fvec3(0.f, 20.f, 0.f));
		add.setFadeLength(50);
		add.setFadeStart(1000);
		add.initTexture(shadowCubeSampler);
	}

	//TODO: LAST NULL LIGHT FOR TILE CULLING OVERRUN

	lightManager.updateAllPointLights();

	const int nr2 = NUM_SPOT_LIGHTS;
	for (int i = 0; i < nr2; ++i)
	{
		auto& add = lightManager.addSpotLight();

		add.setDirection(glm::fvec3(0.f, 0.f, 1.f));
		add.setColour(glm::fvec3(2.f, 2.f, 2.f));
		add.setInnerSpread(glm::radians(10.f));
		add.setOuterSpread(glm::radians(30.f));
		add.setLinear(0.00001);
		add.setQuadratic(0.005);
		add.setPosition(glm::fvec3(-100.f + 50.f * i, 10.f, 0.f));
		add.updateRadius();
		add.setFadeLength(50);
		add.setFadeStart(1000);
		add.initTexture(shadowSampler);
	}

	lightManager.updateAllSpotLights();

	tileCullShader.use();
	tileCullShader.setPointLightCount(lightManager.pointLightsGPUData.size());
	tileCullShader.setSpotLightCount(lightManager.spotLightsGPUData.size());
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	int res[3] = { 1024, 512, 256 };
	lightManager.sunLight.initTexture(shadowSampler, res);

	fboShadow.create();
	fboShadow.bind();
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
}

inline void Renderer::initialiseSamplers()
{
	defaultSampler.initialiseDefaults();
	defaultSampler.setTextureWrapS(GL_REPEAT);
	defaultSampler.setTextureWrapT(GL_REPEAT);
	defaultSampler.setTextureMinFilter(GL_LINEAR_MIPMAP_LINEAR);
	//defaultSampler.setTextureMinFilter(GL_LINEAR);
	defaultSampler.setTextureMagFilter(GL_LINEAR);
	defaultSampler.setTextureLODBias(0);
	defaultSampler.setTextureCompareMode(GL_NONE);
	defaultSampler.setTextureAnisotropy(16);
	defaultSampler.bind(0);
	defaultSampler.bind(1);
	defaultSampler.bind(2);
	defaultSampler.bind(3);

	billboardSampler.initialiseDefaults();
	billboardSampler.setTextureWrapS(GL_CLAMP_TO_EDGE);
	billboardSampler.setTextureWrapT(GL_CLAMP_TO_EDGE);
	billboardSampler.setTextureMinFilter(GL_LINEAR);
	billboardSampler.setTextureMagFilter(GL_LINEAR);
	billboardSampler.setTextureLODBias(0);
	billboardSampler.setTextureCompareMode(GL_NONE);
	billboardSampler.bind(6);

	postSampler.setTextureWrapS(GL_MIRRORED_REPEAT);
	postSampler.setTextureWrapT(GL_MIRRORED_REPEAT);
	postSampler.setTextureMinFilter(GL_LINEAR);
	postSampler.setTextureMagFilter(GL_LINEAR);
	postSampler.setTextureCompareMode(GL_NONE);
	//postSampler.setTextureAnisotropy(16);
	
	//postSampler.bind(2);
	//postSampler.bind(3);
	postSampler.bind(4);
	postSampler.bind(5);

	cubeSampler.setTextureWrapS(GL_CLAMP_TO_EDGE);
	cubeSampler.setTextureWrapT(GL_CLAMP_TO_EDGE);
	cubeSampler.setTextureWrapR(GL_CLAMP_TO_EDGE);
	cubeSampler.setTextureMinFilter(GL_LINEAR);
	cubeSampler.setTextureMagFilter(GL_LINEAR);
	cubeSampler.bind(15);
	
	textSampler.setTextureWrapS(GL_CLAMP_TO_EDGE);
	textSampler.setTextureWrapT(GL_CLAMP_TO_EDGE);
	textSampler.setTextureWrapR(GL_CLAMP_TO_EDGE);
	textSampler.setTextureMinFilter(GL_LINEAR);
	textSampler.setTextureMagFilter(GL_LINEAR);
	textSampler.bind(12);

	shadowSampler.setTextureMagFilter(GL_NEAREST);
	shadowSampler.setTextureMinFilter(GL_NEAREST);
	shadowSampler.setTextureWrapS(GL_CLAMP_TO_BORDER);
	shadowSampler.setTextureWrapT(GL_CLAMP_TO_BORDER);
	shadowSampler.setTextureBorderColour(1.f, 1.f, 1.f, 1.f);
	shadowSampler.bind(14);

	shadowCubeSampler.setTextureMagFilter(GL_NEAREST);
	shadowCubeSampler.setTextureMinFilter(GL_NEAREST);
	shadowCubeSampler.setTextureWrapS(GL_CLAMP_TO_EDGE);
	shadowCubeSampler.setTextureWrapT(GL_CLAMP_TO_EDGE);
	shadowCubeSampler.setTextureWrapR(GL_CLAMP_TO_EDGE);
	shadowCubeSampler.bind(15);

	auto makeHandleResident = [&](Texture2D& tex) -> void {
		if (!tex.glData)
			return;
		auto handle = tex.glData->getHandle(defaultSampler.getGLID());
		glMakeTextureHandleResidentARB(handle);
	};

	///TODO: not all world will use all textures in the texture store, keep track of what needs and what doesnt need to be resident
	for (auto itr = Engine::assets.getTextureList().begin(); itr != Engine::assets.getTextureList().end(); ++itr)
	{
		makeHandleResident(itr->second);
	}
}

void Renderer::initialiseShaders()
{
	shaderStore.loadShader(&tileCullShader);
	shaderStore.loadShader(&gBufferShaderTex);
	shaderStore.loadShader(&bilatBlurShader);
	shaderStore.loadShader(&frustCullShader);
	shaderStore.loadShader(&ssaoShader);
	shaderStore.loadShader(&gBufferShaderMultiTex);
	shaderStore.loadShader(&spotShadowPassShader);
	shaderStore.loadShader(&pointShadowPassShader);
	shaderStore.loadShader(&shape2DShader);
	shaderStore.loadShader(&shape2DShaderNoTex);
	shaderStore.loadShader(&shape3DShader);
	shaderStore.loadShader(&textShader);

	shaderStore.loadShader(ShaderProgram::VertFrag, "wireframe");
	shaderStore.loadShader(ShaderProgram::VertFrag, "Standard");
	shaderStore.loadShader(ShaderProgram::VertFrag, "test");
}

void Renderer::initialiseFramebuffers()
{
	initialiseGBuffer();
	initialiseSSAOBuffer();
	initialiseScreenFramebuffer();
}

void Renderer::reInitialiseFramebuffers()
{
	destroyFramebufferTextures();
	initialiseFramebuffers();
	lightPassTex.release();
	lightPassTex.createFromStream(GL_RGBA32F, Engine::cfg.render.resolution.x, Engine::cfg.render.resolution.y, GL_RGBA, GL_FLOAT, NULL);
}

void Renderer::destroyFramebufferTextures()
{
	fboGBuffer.destroyAllAttachments();
	fboScreen.destroyAllAttachments();
	fboSSAO.destroyAllAttachments();
	fboSSAOBlur.destroyAllAttachments();
}

void Renderer::setActiveCam(Camera & pCam)
{
	activeCam = &pCam;
	cameraProjUpdated();

	// Indices for corners of the view
	//TL 4  5
	//TR 10 11
	//BR 16 17

	//BR 22 23
	//BL 28 29
	//TL 34 35

	quadVerticesViewRays[4] = activeCam->viewRays2[3].x;
	quadVerticesViewRays[5] = activeCam->viewRays2[3].y;

	quadVerticesViewRays[10] = activeCam->viewRays2[2].x;
	quadVerticesViewRays[11] = activeCam->viewRays2[2].y;

	quadVerticesViewRays[16] = activeCam->viewRays2[1].x;
	quadVerticesViewRays[17] = activeCam->viewRays2[1].y;

	quadVerticesViewRays[22] = activeCam->viewRays2[1].x;
	quadVerticesViewRays[23] = activeCam->viewRays2[1].y;

	quadVerticesViewRays[28] = activeCam->viewRays2[0].x;
	quadVerticesViewRays[29] = activeCam->viewRays2[0].y;

	quadVerticesViewRays[34] = activeCam->viewRays2[3].x;
	quadVerticesViewRays[35] = activeCam->viewRays2[3].y;
}

void Renderer::cameraProjUpdated()
{
	gBufferShaderTex.setProj(activeCam->proj);
	gBufferShaderMultiTex.setProj(activeCam->proj);
	ssaoShader.setProj(activeCam->proj);
	ssaoShader.setViewport(glm::ivec2(viewport.width, viewport.height));
	frustCullShader.setProj(activeCam->proj);
}

/*void Renderer::bakeStaticLights()
{
	For each light render the scene to depth texture
} */

inline void Renderer::initialiseScreenQuad()
{
	vaoQuad.create();
	vboQuad.create();

	vboQuad.bind();
	vboQuad.bufferData(sizeof(quadVertices), (void*)quadVertices, GL_STATIC_DRAW);
	
	vaoQuad.bind();
	vaoQuad.addAttrib("position", 2, GL_FLOAT);
	vaoQuad.addAttrib("texCoord", 2, GL_FLOAT);
	vaoQuad.enableFor(bilatBlurShader);
	vaoQuad.enableFor(ssaoShader);


	vaoQuadViewRays.create();
	vboQuadViewRays.create();

	vboQuadViewRays.bind();
	vboQuadViewRays.bufferData(sizeof(quadVerticesViewRays), quadVerticesViewRays, GL_STATIC_DRAW);
	
	vaoQuadViewRays.bind();
	vaoQuadViewRays.addAttrib("position", 2, GL_FLOAT);
	vaoQuadViewRays.addAttrib("texCoord", 2, GL_FLOAT);
	vaoQuadViewRays.addAttrib("viewRay", 2, GL_FLOAT);
	vaoQuadViewRays.enableFor(*shaderStore.getShader("Standard"));
	vaoQuadViewRays.enableFor(*shaderStore.getShader("test"));
}
