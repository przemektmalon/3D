#pragma once
#include "Texture.hpp"
#include "BufferObjects.hpp"
#include "Sampler.hpp"
#include "Billboard.hpp"

struct DirectLightData
{
	DirectLightData() {}
	DirectLightData(glm::fvec3 dir, glm::fvec3 col) : direction(dir), colour(col) {}
	~DirectLightData() {}

	glm::fvec3 direction, colour;
};

class FBO;

class PointLight
{
	friend class LightManager;
public:
	struct GPUData;

private:
	GPUData* gpuData;
	GLTextureCube shadowTex;
	FBO* fbo;

	float shadowRenderDistance;

	glm::fmat4 proj;
	bool matNeedsUpdate;

public:
	PointLight() : matNeedsUpdate(true), gpuData(nullptr), fbo(nullptr), shadowRenderDistance(10000) {}

	struct GPUData
	{
		friend struct PointLight;
		friend class LightManager;
	private:

		union PositionRadius
		{
			PositionRadius() : all(0, 0, 0, 0) {}
			struct Separate
			{
				glm::fvec3 position;
				float radius;
			} separate;
			glm::fvec4 all;
		} positionRadius;

		union ColourQuadratic
		{
			ColourQuadratic() : all(1, 0, 1, 0.001) {}
			struct Separate
			{
				glm::fvec3 colour;
				float quadratic;
			} separate;
			glm::fvec4 all;
		} colourQuadratic;

		union LinearFadesTexture
		{
			LinearFadesTexture() { separate.linear = 0.001; separate.fadeStart = 500; separate.fadeLength = 100; separate.textureHandle = 0; }
			struct Separate
			{
				float linear;
				unsigned short fadeStart;
				unsigned short fadeLength;
				GLuint64 textureHandle;
			} separate;
			glm::fvec4 all;
		} linearFadesTexture;

		glm::fmat4 projView[6];

	protected:
		float getRadius() { return positionRadius.separate.radius; }
		void setRadius(float set) {
			positionRadius.separate.radius = set;
		}
		glm::fvec3 getPosition() { return positionRadius.separate.position; }
		void setPosition(glm::fvec3 set) {
			positionRadius.separate.position = set;
		}
		glm::fvec3 getColour() { return colourQuadratic.separate.colour; }
		void setColour(glm::fvec3 set) {
			colourQuadratic.separate.colour = set;
		}
		float getQuadratic() { return colourQuadratic.separate.quadratic; }
		void setQuadratic(float set) {
			colourQuadratic.separate.quadratic = set;
		}
		float getLinear() { return linearFadesTexture.separate.linear; }
		void setLinear(float set) {
			linearFadesTexture.separate.linear = set;
		}
		float getFadeStart() { return linearFadesTexture.separate.fadeStart; }
		void setFadeStart(float set) {
			linearFadesTexture.separate.fadeStart = set;
		}
		float getFadeLength() { return linearFadesTexture.separate.fadeLength; }
		void setFadeLength(float set) {
			linearFadesTexture.separate.fadeLength = set;
		}
		GLuint64 getHandle() { return linearFadesTexture.separate.textureHandle; }
		void setHandle(GLuint64 set) {
			linearFadesTexture.separate.textureHandle = set;
		}
		glm::fmat4* getProjView() { return projView; }
		void updateProjView(glm::fmat4& proj) ///TODO: dont need lookAt, just construct manually
		{
			projView[0] = proj * (glm::lookAt(getPosition(), getPosition() + glm::fvec3(1.0, 0.0, 0.0), glm::fvec3(0.0, -1.0, 0.0)));
			projView[1] = proj * (glm::lookAt(getPosition(), getPosition() + glm::fvec3(-1.0, 0.0, 0.0), glm::fvec3(0.0, -1.0, 0.0)));
			projView[2] = proj * (glm::lookAt(getPosition(), getPosition() + glm::fvec3(0.0, 1.0, 0.0), glm::fvec3(0.0, 0.0, 1.0)));
			projView[3] = proj * (glm::lookAt(getPosition(), getPosition() + glm::fvec3(0.0, -1.0, 0.0), glm::fvec3(0.0, 0.0, -1.0)));
			projView[4] = proj * (glm::lookAt(getPosition(), getPosition() + glm::fvec3(0.0, 0.0, 1.0), glm::fvec3(0.0, -1.0, 0.0)));
			projView[5] = proj * (glm::lookAt(getPosition(), getPosition() + glm::fvec3(0.0, 0.0, -1.0), glm::fvec3(0.0, -1.0, 0.0)));
		}
	};

	float getRadius() { return gpuData->getRadius(); }
	glm::fvec3 getPosition() { return gpuData->getPosition(); }
	glm::fvec3 getColour() { return gpuData->getColour(); }
	float getQuadratic() { return gpuData->getQuadratic(); }
	float getLinear() { return gpuData->getLinear(); }
	float getFadeStart() { return gpuData->getFadeStart(); }
	float getFadeLength() { return gpuData->getFadeLength(); }
	GLuint64 getHandle() { return gpuData->getHandle(); }
	GLTextureCube& getShadowTexture() { return shadowTex; }
	glm::fmat4* getProjView() { return gpuData->projView; }

	void setPosition(glm::fvec3 set)
	{
		gpuData->setPosition(set);
		matNeedsUpdate = true;
	}

	void setColour(glm::fvec3 set)
	{
		gpuData->setColour(set);
	}

	void setQuadratic(float set)
	{
		gpuData->setQuadratic(set);
		updateRadius();
		matNeedsUpdate = true;
	}

	void setLinear(float set)
	{
		gpuData->setLinear(set);
		updateRadius();
		matNeedsUpdate = true;
	}

	void setFadeStart(float set)
	{
		gpuData->setFadeStart(set);
	}

	void setFadeLength(float set)
	{
		gpuData->setFadeLength(set);
	}

	void update()
	{
		if (matNeedsUpdate)
			gpuData->updateProjView(proj);
	}

	void initTexture(Sampler& sampler, int resolution = 1024)
	{
		shadowTex.createFromStream(GL_DEPTH_COMPONENT32F_NV, resolution, resolution, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		gpuData->setHandle(shadowTex.makeResident(sampler.getGLID()));
	}

	void updateRadius();
	inline static float calculateRadius(float linear, float quad);

};

class SpotLight
{
	friend class LightManager;
public:
	struct GPUData;
	
private:
	bool matNeedsUpdate;
	GPUData* gpuData;
	GLTexture2D shadowTex;

public:
	SpotLight() : matNeedsUpdate(true), gpuData(nullptr) {}
	~SpotLight() {}

	struct GPUData
	{
		friend struct SpotLight;
		friend class LightManager;
	private:
		union PositionRadius
		{
			PositionRadius() : all(0, 0, 0, 0) {}
			struct Separate
			{
				glm::fvec3 position;
				float radius;
			} separate;
			glm::fvec4 all;
		} positionRadius;

		union DirectionInner
		{
			DirectionInner() : all(0, 0, 0, 0) {}
			struct Separate
			{
				glm::fvec3 direction;
				float innerSpread;
			} separate;
			glm::fvec4 all;
		} directionInner;

		union ColourOuter
		{
			ColourOuter() : all(0, 0, 0, 0) {}
			struct Separate
			{
				glm::fvec3 colour;
				float outerSpread;
			} separate;
			glm::fvec4 all;
		} colourOuter;

		union LinearQuadraticHandle
		{
			LinearQuadraticHandle() { separate.linear = 0.001; separate.quadratic = 0.001; separate.textureHandle = 0; }
			struct Separate
			{
				float linear;
				float quadratic;
				GLuint64 textureHandle;
			} separate;
			glm::fvec4 all;
		} linearQuadraticHandle;

		union Fades
		{
			Fades() { separate.fadeStart = 500; separate.fadeLength = 100; }
			struct Separate
			{
				unsigned short fadeStart;
				unsigned short fadeLength;
				unsigned int na;
				unsigned int na2;
				unsigned int na3;
			} separate;
			glm::ivec4 all;
		} fades;

		glm::fmat4 projView;

	protected:

		float getRadius() { return positionRadius.separate.radius; }
		void setRadius(float set) {
			positionRadius.separate.radius = set;
		}
		glm::fvec3 getPosition() { return positionRadius.separate.position; }
		void setPosition(glm::fvec3 set) {
			positionRadius.separate.position = set;
		}
		glm::fvec3 getDirection() { return directionInner.separate.direction; }
		void setDirection(glm::fvec3 set) {
			directionInner.separate.direction = set;
		}
		float getInner() { return directionInner.separate.innerSpread; }
		bool setInner(float set) {
			directionInner.separate.innerSpread = set;
			if (getInner() > getOuter()) {
				colourOuter.separate.outerSpread = set; return true;
			}
			return false;
		}
		glm::fvec3 getColour() { return colourOuter.separate.colour; }
		void setColour(glm::fvec3 set) {
			colourOuter.separate.colour = set;
		}
		float getOuter() { return colourOuter.separate.outerSpread; }
		bool setOuter(float set) {
			colourOuter.separate.outerSpread = set;
			if (getOuter() < getInner()) {
				directionInner.separate.innerSpread = set; return true;
			}
			return false;
		}
		float getQuadratic() { return linearQuadraticHandle.separate.quadratic; }
		void setQuadratic(float set) {
			linearQuadraticHandle.separate.quadratic = set;
		}
		float getLinear() { return linearQuadraticHandle.separate.linear; }
		void setLinear(float set) {
			linearQuadraticHandle.separate.linear = set;
		}
		GLuint64 getHandle() { return linearQuadraticHandle.separate.textureHandle; }
		void setHandle(GLuint64 set) {
			linearQuadraticHandle.separate.textureHandle = set;
		}
		float getFadeStart() { return fades.separate.fadeStart; }
		void setFadeStart(float set) {
			fades.separate.fadeStart = set;
		}
		float getFadeLength() { return fades.separate.fadeLength; }
		void setFadeLength(float set) {
			fades.separate.fadeLength = set;
		}

		glm::fmat4& getProjView() { return projView; }
		void updateProjView() ///TODO: dont need lookAt, just construct manually
		{
			projView = glm::perspective<float>(getOuter(), 1.f, 0.01f, getRadius() * 2.f) * glm::lookAt(getPosition(), getPosition() + getDirection(), glm::fvec3(0, 1, 0));
		}
	};

	float getRadius() { return gpuData->getRadius(); }
	glm::fvec3 getPosition() { return gpuData->getPosition(); }
	glm::fvec3 getDirection() { return gpuData->getDirection(); }
	glm::fvec3 getColour() { return gpuData->getColour(); }
	float getInner() { return gpuData->getInner(); }
	float getOuter() { return gpuData->getOuter(); }
	float getQuadratic() { return gpuData->getQuadratic(); }
	float getLinear() { return gpuData->getLinear(); }
	float getFadeStart() { return gpuData->getFadeStart(); }
	float getFadeLength() { return gpuData->getFadeLength(); }
	GLuint64 getHandle() { return gpuData->getHandle(); }
	GLTexture2D& getShadowTexture() { return shadowTex; }
	glm::fmat4& getProjView() { return gpuData->projView; }

	void setPosition(glm::fvec3 set)
	{
		gpuData->setPosition(set);
		matNeedsUpdate = true;
	}

	void setDirection(glm::fvec3 set)
	{
		gpuData->setDirection(set);
		matNeedsUpdate = true;
	}

	void setInnerSpread(float set)
	{
		if (gpuData->setInner(glm::cos(set)))
			matNeedsUpdate = true;
	}

	void setOuterSpread(float set)
	{
		gpuData->setOuter(glm::cos(set));
		matNeedsUpdate = true;
	}

	void setColour(glm::fvec3 set)
	{
		gpuData->setColour(set);
	}

	void setQuadratic(float set)
	{
		gpuData->setQuadratic(set);
		updateRadius();
		matNeedsUpdate = true;
	}

	void setLinear(float set)
	{
		gpuData->setLinear(set);
		updateRadius();
		matNeedsUpdate = true;
	}

	void setFadeStart(float set)
	{
		gpuData->setFadeStart(set);
	}

	void setFadeLength(float set)
	{
		gpuData->setFadeLength(set);
	}

	void update()
	{
		if (matNeedsUpdate)
			gpuData->updateProjView();
	}

	void initTexture(Sampler& sampler, int resolution = 512)
	{
		shadowTex.createFromStream(GL_DEPTH_COMPONENT32F_NV, resolution, resolution, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		gpuData->setHandle(shadowTex.makeResident(sampler.getGLID()));
	}

	void updateRadius();
	inline static float calculateRadius(float linear, float quad);
};

class LightManager
{
public: ///TODO: Max light count is 500, add setters etc
	LightManager() 
	{
		spotLightsBuffer.create();
		pointLightsBuffer.create();

		spotLightsGPUData.reserve(500);
		pointLightsGPUData.reserve(500);
	}
	~LightManager() {}

	void drawLightIcons()
	{
		int i = 0;
		for (auto& b : pointLightIcons)
		{
			b.pos = pointLights[i].getPosition();
			b.draw();
			++i;
		}

		i = 0;
		for (auto& b : spotLightIcons)
		{
			b.pos = spotLights[i].getPosition();
			b.draw();
			++i;
		}
	}

	PointLight& addPointLight()
	{
		auto data = PointLight::GPUData();
		return addPointLight(data);
	}

	PointLight& addPointLight(PointLight::GPUData& data);

	PointLight* getPointLight(u32 pIndex)
	{
		return pointLights.data() + pIndex;
	}

	void updatePointLight(u32 pIndex)
	{
		auto data = (PointLight::GPUData*)pointLightsBuffer.mapRange(pIndex * sizeof(PointLight::GPUData), sizeof(PointLight::GPUData), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
		data->positionRadius.all = pointLightsGPUData[pIndex].positionRadius.all;
		data->colourQuadratic.all = pointLightsGPUData[pIndex].colourQuadratic.all;
		pointLightsBuffer.unmap();
	}

	void updateAllPointLights()
	{
		for (auto& l : pointLights)
			l.update();
		pointLightsBuffer.bufferData(sizeof(PointLight::GPUData) * pointLightsGPUData.size(), pointLightsGPUData.data(), GL_DYNAMIC_DRAW);
	}

	SpotLight& addSpotLight()
	{
		auto data = SpotLight::GPUData();
		return addSpotLight(data);
	}

	SpotLight& addSpotLight(SpotLight::GPUData& data);

	SpotLight* getSpotLight(u32 pIndex)
	{
		return spotLights.data() + pIndex;
	}

	void updateSpotLight(u32 pIndex)
	{
		auto data = (SpotLight::GPUData*)spotLightsBuffer.mapRange(pIndex * sizeof(SpotLight::GPUData), sizeof(SpotLight::GPUData), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
		data->positionRadius.all = spotLights[pIndex].gpuData->positionRadius.all;
		data->directionInner.all = spotLights[pIndex].gpuData->directionInner.all;
		data->colourOuter.all = spotLights[pIndex].gpuData->colourOuter.all;
		data->linearQuadraticHandle.all = spotLights[pIndex].gpuData->linearQuadraticHandle.all;
		spotLightsBuffer.unmap();
	}

	void updateAllSpotLights()
	{
		for (auto& l : spotLights)
			l.update();
		spotLightsBuffer.bufferData(sizeof(SpotLight::GPUData) * spotLightsGPUData.size(), spotLightsGPUData.data(), GL_DYNAMIC_DRAW);
	}

	std::vector<PointLight> pointLights;
	std::vector<PointLight::GPUData> pointLightsGPUData;
	SSBO pointLightsBuffer;

	std::vector<SpotLight> spotLights;
	std::vector<SpotLight::GPUData> spotLightsGPUData;
	SSBO spotLightsBuffer;

	std::vector<DirectLightData> directLights;

	std::vector<Billboard> pointLightIcons;
	std::vector<Billboard> spotLightIcons;
	
	//std::vector<SpotLight::GPUData> staticSpotLights;
	//std::vector<PointLight::GPUData> staticPointLightsGPUData;
	//std::vector<GLTextureCube> staticPointLightsShadowTex;
};
