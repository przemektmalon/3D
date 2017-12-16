#include "Lights.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "Framebuffer.hpp"
#include "AssetManager.hpp"
#include "Billboard.hpp"

PointLight::PointLight() { }

void PointLight::updateRadius()
{
	gpuData->radius = (0.5 * (std::sqrtf(gpuData->linear*gpuData->linear + (Engine::cfg.render.minimumLightConstant * gpuData->quadratic) - gpuData->linear))) / gpuData->quadratic;
	updateProj();
	updateProjView();
}

inline float PointLight::calculateRadius(float linear, float quad)
{
	return  (0.5 * (std::sqrtf(linear*linear + (Engine::cfg.render.minimumLightConstant * quad) - linear))) / quad;
}

void SpotLight::updateRadius()
{
	//gpuData->radius = (0.5 * (std::sqrtf(gpuData->linear*gpuData->linear + (Engine::cfg.render.minimumLightConstant * gpuData->quadratic) - gpuData->linear))) / (gpuData->quadratic * 2.f);
	gpuData->radius = calculateRadius(gpuData->linear, gpuData->quadratic);
	updateProj();
	updateProjView();
}

inline float SpotLight::calculateRadius(float linear, float quad)
{
	return (0.5 * (std::sqrtf(linear*linear + (Engine::cfg.render.minimumLightConstant * quad) - linear))) / (quad * 2.f);
}

PointLight & LightManager::addPointLight(PointLight::GPUData& data)
{
	pointLights.push_back(PointLight());
	auto& light = pointLights.back();
	pointLightsGPUData.push_back(data);
	light.gpuData = &pointLightsGPUData.back();

	pointLightIcons.push_back(Billboard(data.position, glm::fvec2(2, 2), Engine::assets.get2DTex("light")->glData));
	pointLightIcons.back().initGL();

	return light;
}

SpotLight & LightManager::addSpotLight(SpotLight::GPUData& data)
{
	spotLights.push_back(SpotLight());
	auto& light = spotLights.back();
	spotLightsGPUData.push_back(data);
	light.gpuData = &spotLightsGPUData.back();

	spotLightIcons.push_back(Billboard(data.position, glm::fvec2(2, 2), Engine::assets.get2DTex("light")->glData));
	spotLightIcons.back().initGL();

	return light;
}


