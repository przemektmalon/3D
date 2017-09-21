#include "Lights.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "Framebuffer.hpp"
#include "AssetManager.hpp"
#include "Billboard.hpp"

PointLight::PointLight()
{
	//shadowTex.createFromStream(1024, 1024, GL_DEPTH_COMPONENT32F_NV, GL_DEPTH_COMPONENT, GL_FLOAT);
	//shadowTex.createFromStream(1024, 1024, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
	//fbo = &Engine::r->fboLight[3];
}

PointLight & LightManager::addPointLight(PointLight::GPUData & data)
{
	pointLights.push_back(PointLight());
	auto& light = pointLights.back();
	pointLightsGPUData.push_back(data);
	light.gpuData = &pointLightsGPUData.back();

	pointLightIcons.push_back(Billboard(data.position, glm::fvec2(2, 2), Engine::assets.get2DTex("light")->glData));
	pointLightIcons.back().initGL();

	return light;
}

SpotLight & LightManager::addSpotLight(SpotLight::GPUData data)
{
	spotLights.push_back(SpotLight());
	auto& light = spotLights.back();
	spotLightsGPUData.push_back(data);
	light.gpuData = &spotLightsGPUData.back();

	spotLightIcons.push_back(Billboard(data.position, glm::fvec2(2, 2), Engine::assets.get2DTex("light")->glData));
	spotLightIcons.back().initGL();

	return light;
}