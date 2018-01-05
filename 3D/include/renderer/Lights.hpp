#pragma once
#include "Texture.hpp"
#include "BufferObjects.hpp"
#include "Sampler.hpp"
#include "Billboard.hpp"

class Light
{
	friend class LightManager;
public:
	GLTexture* shadowTex;

	virtual GLTexture* getShadowTexture() = 0;
};

class LightGPUDataBase
{
	friend class LightManager;
public:
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
		ColourQuadratic() : all(0, 0, 0, 0) {}
		struct Separate
		{
			glm::fvec3 colour;
			float quadratic;
		} separate;
		glm::fvec4 all;
	} colourQuadratic;

	union LinearFadesTexture
	{
		LinearFadesTexture() { separate.linear = 0.001; separate.fadeStart = 500; separate.fadeLength = 100; separate.shadowHandle = 0; }
		struct Separate
		{
			float linear;
			unsigned short fadeStart;
			unsigned short fadeLength;
			GLuint64 shadowHandle;
		} separate;
		glm::fvec4 all;
	} linearFadesTexture;

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
	GLuint64 getHandle() { return linearFadesTexture.separate.shadowHandle; }
	void setHandle(GLuint64 set) {
		linearFadesTexture.separate.shadowHandle = set;
	}
};

class SunLight : public Light
{
	friend class LightManager;
public:
	struct GPUData;

private:
	GPUData* gpuData;

public:

	GLTexture* getShadowTexture() { return shadowTex; }

	struct GPUData
	{
		friend struct SunLight;
		friend class LightManager;
	protected:
		void setHandle(int index, GLuint64 pHandle)
		{
			shadowHandles[index] = pHandle;
		}

	private:
		union Colour
		{
			Colour() : all(100, 100, 100, 1) {}
			struct Separate
			{
				glm::fvec3 colour;
			} separate;
			glm::fvec4 all;
		} colour;

		union Direction
		{
			Direction() : all(-1, -1, -1, 0) 
			{
				separate.direction = glm::normalize(glm::fvec3(0.1, -1.0, 0.2));
			}
			struct Separate
			{
				glm::fvec3 direction;
			} separate;
			glm::fvec4 all;
		} direction;

	public:
		GLuint64 shadowHandles[3];
		glm::fmat4 projView[3];
		float cascadeEnds[4];
		glm::fvec4 box[2];
	};

	SunLight()
	{
		shadowTex = new GLTexture2D[3];
		gpuData = new GPUData();
		gpuData->cascadeEnds[0] = 250;
		gpuData->cascadeEnds[1] = 1050;
		gpuData->cascadeEnds[2] = 3000;
		gpuData->cascadeEnds[3] = 5000;
	}

	void initTexture(Sampler& sampler, int* resolution)
	{
		for (int i = 0; i < 3; ++i)
		{
			((GLTexture2D*)(shadowTex)+i)->createFromStream(GL_DEPTH_COMPONENT32F_NV, resolution[i], resolution[i], GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			gpuData->setHandle(i, (((GLTexture2D*)shadowTex)+i)->makeResident(sampler.getGLID()));
		}
	}

	glm::fvec3 getDirection() { return gpuData->direction.separate.direction; }

	glm::fmat4* getProjView() { return gpuData->projView; }
	void setProjView(int index, glm::fmat4& projView) { gpuData->projView[index] = projView; }
	GPUData* getData() { return gpuData; }
};

class PointLight : public Light
{
	friend class LightManager;
public:
	struct GPUData;

private:
	GPUData* gpuData;

	glm::fmat4 proj;
	bool matNeedsUpdate;

public:
	PointLight() : matNeedsUpdate(true), gpuData(nullptr)
	{
		shadowTex = new GLTextureCube();
	}

	struct GPUData : public LightGPUDataBase
	{
		friend struct PointLight;
		friend class LightManager;
	private:

		glm::fmat4 projView[6];

	protected:
		
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
	GLTextureCube* getShadowTexture() { return (GLTextureCube*)shadowTex; }
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
		((GLTextureCube*)shadowTex)->createFromStream(GL_DEPTH_COMPONENT32F_NV, resolution, resolution, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		gpuData->setHandle(shadowTex->makeResident(sampler.getGLID()));
	}

	void updateRadius();
	inline static float calculateRadius(float linear, float quad);

};

class SpotLight : public Light
{
	friend class LightManager;
public:
	struct GPUData;
	
private:
	bool matNeedsUpdate;
	GPUData* gpuData;

public:
	SpotLight() : matNeedsUpdate(true), gpuData(nullptr) {}
	~SpotLight() {}

	struct GPUData : public LightGPUDataBase
	{
		friend struct SpotLight;
		friend class LightManager;
	private:

		union DirectionInner
		{
			DirectionInner() : all(0, 0, 0, 0) {}
			struct Separate
			{
				glm::fvec3 direction;
				float inner;
			} separate;
			glm::fvec4 all;
		} directionInner;

		union Outer
		{
			Outer() : all(0, 0, 0, 0) {}
			struct Separate
			{
				float outer;
			} separate;
			glm::fvec4 all;
		} outer;

		glm::fmat4 projView;

	protected:

		glm::fvec3 getDirection() { return directionInner.separate.direction; }
		void setDirection(glm::fvec3 set) {
			directionInner.separate.direction = set;
		}
		float getInner() { return colourQuadratic.separate.quadratic; }
		bool setInner(float set) {
			directionInner.separate.inner = set;
			if (getInner() > getOuter()) {
				outer.separate.outer = set; return true;
			}
			return false;
		}
		float getOuter() { return outer.separate.outer; }
		bool setOuter(float set) {
			outer.separate.outer = set;
			if (getOuter() < getInner()) {
				directionInner.separate.inner = set; return true;
			}
			return false;
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
	GLTexture2D* getShadowTexture() { return (GLTexture2D*)shadowTex; }
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
		((GLTexture2D*)shadowTex)->createFromStream(GL_DEPTH_COMPONENT32F_NV, resolution, resolution, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		gpuData->setHandle(shadowTex->makeResident(sampler.getGLID()));
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

		sunLightBuffer.create();
		sunLightBuffer.bufferData(sizeof(SunLight::GPUData), 0, GL_STREAM_DRAW);
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
		data->colourQuadratic.all = spotLights[pIndex].gpuData->colourQuadratic.all;
		data->directionInner.all = spotLights[pIndex].gpuData->directionInner.all;
		data->linearFadesTexture.all = spotLights[pIndex].gpuData->linearFadesTexture.all;
		data->outer.separate.outer = spotLights[pIndex].gpuData->outer.separate.outer;
		spotLightsBuffer.unmap();
	}

	void updateAllSpotLights()
	{
		for (auto& l : spotLights)
			l.update();
		spotLightsBuffer.bufferData(sizeof(SpotLight::GPUData) * spotLightsGPUData.size(), spotLightsGPUData.data(), GL_DYNAMIC_DRAW);
	}

	void updateSunLight()
	{
		/*auto data = (SunLight::GPUData*)sunLightBuffer.mapRange(0, sizeof(SunLight::GPUData), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
		data->colour.all = sunLight.gpuData->colour.all;
		data->direction.all = sunLight.gpuData->direction.all;
		for (int i = 0; i < 3; ++i) {
			data->shadowHandles[i] = sunLight.gpuData->shadowHandles[i];
			data->projView[i][0] = sunLight.gpuData->projView[i][3];
			data->projView[i][1] = sunLight.gpuData->projView[i][2];
			data->projView[i][2] = sunLight.gpuData->projView[i][1];
			data->projView[i][3] = sunLight.gpuData->projView[i][0];
		}
		for (int i = 0; i < 4; ++i)
			data->cascadeEnds[i] = sunLight.gpuData->cascadeEnds[i];
		data->box[0] = sunLight.gpuData->box[0];
		data->box[1] = sunLight.gpuData->box[1];
		
		data->cascadeEnds[3] = 1000;

		sunLightBuffer.unmap();*/
		sunLightBuffer.bind();
		sunLightBuffer.bufferData(sizeof(SunLight::GPUData), sunLight.gpuData, GL_STREAM_DRAW);
	}

	std::vector<PointLight> pointLights;
	std::vector<PointLight::GPUData> pointLightsGPUData;
	SSBO pointLightsBuffer;

	std::vector<SpotLight> spotLights;
	std::vector<SpotLight::GPUData> spotLightsGPUData;
	SSBO spotLightsBuffer;

	//std::vector<DirectLightData> directLights;

	std::vector<Billboard> pointLightIcons;
	std::vector<Billboard> spotLightIcons;
	
	SunLight sunLight;
	SSBO sunLightBuffer;

	//std::vector<SpotLight::GPUData> staticSpotLights;
	//std::vector<PointLight::GPUData> staticPointLightsGPUData;
	//std::vector<GLTextureCube> staticPointLightsShadowTex;
};
