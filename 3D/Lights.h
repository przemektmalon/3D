#pragma once
#include "Include.h"

//Minimum light constant. 
//Higher Constant == LOWER MINUMUM LIGHT
//Lower Constant == HIGHER MINIMUM LIGHT
#define MIN_LIGHT_CONSTANT 700.f

struct PointLightData
{
public:
	PointLightData() {}
	PointLightData(glm::fvec3 pos, glm::fvec3 col, float lin = 0.0001f, float quad = 0.0003f) : positionRad(glm::fvec4(pos, lin))/*, colourQuad(glm::fvec4(col,quad))*/ {}
	~PointLightData() {}

	union
	{
		struct
		{
			glm::fvec3 position;
			float radius;
		};
		glm::fvec4 positionRad;
	};

	union
	{
		struct
		{
			glm::fvec3 colour;
			float quadratic;
		};
		glm::fvec4 colourQuad;
	};

	union
	{
		struct
		{
			float linear;
			float DATA1;
			float DATA2;
			float DATA3;
		};
		glm::fvec4 linearData;
	};

	void updateRadius()
	{
		//radius = (0.5 * (std::sqrtf(linear*linear + (396.f * quadratic) - linear))) / quadratic;
		radius = (0.5 * (std::sqrtf(linear*linear + (MIN_LIGHT_CONSTANT * quadratic) - linear))) / quadratic;

	}
};

struct SpotLightData
{
	SpotLightData() {}
	SpotLightData(glm::fvec3 pos, glm::fvec3 dir, glm::fvec3 col, float inner, float outer = 0.f, float lin = 0.0001f, float quad = 0.0003f) : position(pos), direction(dir), colour(col), linear(lin), quadratic(quad), innerSpread(inner), outerSpread(outer) {
		if (outer < inner) { outer = inner; }
	}
	~SpotLightData() {}

	union
	{
		struct
		{
			glm::fvec3 position;
			float radius;
		};
		glm::fvec4 posRad;
	};

	union
	{
		struct
		{
			glm::fvec3 direction;
			float innerSpread;
		};
		glm::fvec4 dirInner;
	};

	union
	{
		struct
		{
			glm::fvec3 colour;
			float outerSpread;
		};
		glm::fvec4 colOuter;
	};

	union
	{
		struct
		{
			float linear;
			float quadratic;
			float D3;
			float D4;
		};
		glm::fvec4 linQuad;
	};

	void updateRadius()
	{
		radius = (0.5 * (std::sqrtf(linear*linear + (MIN_LIGHT_CONSTANT * quadratic) - linear))) / (quadratic * 2.f);
	}
};

struct DirectLightData
{
	DirectLightData() {}
	DirectLightData(glm::fvec3 dir, glm::fvec3 col) : direction(dir), colour(col) {}
	~DirectLightData() {}

	glm::fvec3 direction, colour;
};

#define MINIMUM_LIGHT 0.01

class PointLight
{
public:

	float calculateRadius()
	{
		data.radius = (0.5 * (std::sqrtf(data.linear*data.linear + (MIN_LIGHT_CONSTANT * data.quadratic) - data.linear))) / data.quadratic;
		return data.radius;
	}

	inline static float calculateRadius(float linear, float quad)
	{
		return  (0.5 * (std::sqrtf(linear*linear + (MIN_LIGHT_CONSTANT * quad) - linear))) / quad;
	}

	PointLightData data;
};

class SpotLight
{
public:
	SpotLight() {}
	~SpotLight() {}

	float calculateRadius()
	{
		data.radius = (0.5 * (std::sqrtf(data.linear*data.linear + (MIN_LIGHT_CONSTANT * data.quadratic) - data.linear))) / (data.quadratic * 2.f);
		return data.radius;
	}

	inline static float calculateRadius(float linear, float quad)
	{
		return  (0.5 * (std::sqrtf(linear*linear + (MIN_LIGHT_CONSTANT* quad) - linear))) / (quad * 2.f);
	}

	SpotLightData data;
};

class LightManager
{
public:
	LightManager() {}
	~LightManager() {}

	void addPointLight(PointLightData pLight)
	{
		pointLights.push_back(pLight);
	}

	PointLightData* getPointLight(u32 pIndex)
	{
		//return &pointLights[pIndex];
		return pointLights.data() + pIndex;
	}

	void updatePointLight(u32 pIndex)
	{
		auto data = (PointLightData*)pointLightsBuffer.mapRange(pIndex * sizeof(PointLightData), sizeof(PointLightData), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
		data->positionRad = pointLights[pIndex].positionRad;
		data->colourQuad = pointLights[pIndex].colourQuad;
		pointLightsBuffer.unmap();
	}

	void updateAllPointLights()
	{
		//auto data = pointLightsBuffer.mapRange(0, sizeof(PointLightData) * pointLights.size(), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		//memcpy(data, (void*)pointLights.data(), pointLights.size() * sizeof(PointLightData));
		//pointLightsBuffer.unmap();
		pointLightsBuffer.bufferData(sizeof(PointLightData) * pointLights.size(), pointLights.data(), GL_DYNAMIC_DRAW);
	}

	void addSpotLight(SpotLightData pLight)
	{
		spotLights.push_back(pLight);
	}

	SpotLightData* getSpotLight(u32 pIndex)
	{
		return spotLights.data() + pIndex;
	}

	void updateSpotLight(u32 pIndex)
	{
		auto data = (SpotLightData*)spotLightsBuffer.mapRange(pIndex * sizeof(SpotLightData), sizeof(SpotLightData), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
		data->posRad = spotLights[pIndex].posRad;
		data->dirInner = spotLights[pIndex].dirInner;
		data->colOuter = spotLights[pIndex].colOuter;
		data->linQuad = spotLights[pIndex].linQuad;
		spotLightsBuffer.unmap();
	}

	void updateAllSpotLights()
	{
		spotLightsBuffer.bufferData(sizeof(SpotLightData) * spotLights.size(), spotLights.data(), GL_DYNAMIC_DRAW);
	}

	SSBO pointLightsBuffer;
	SSBO spotLightsBuffer;

	std::vector<PointLightData> pointLights;
	std::vector<SpotLightData> spotLights;
	std::vector<DirectLightData> directLights;
};
