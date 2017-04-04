#pragma once
#include <unordered_map>
#include "Mesh.h"
#include "Transform.h"
#include "SAOShader.h"
#include "GBufferShader.h"
#include "Framebuffer.h"
#include "Text.h"
#include "BufferObjects.h"
#include <chrono>
#include <time.h>
#include "Lights.h"
#include "DeferredTileCullComputeShader.h"
#include "Sampler.h"

#define NUM_VALID_RESOLUTIONS 8

typedef struct {
	u32 count;
	u32 instanceCount;
	u32 first;
	//u32 baseInstance;
	float radius;
} GLCMD;

class MasterRenderer
{
public:
	MasterRenderer() : shadScale(1.f)
	{
		rC.ssaoPower = (1.f);
		rC.ssaoScale = (1.f);
	}
	~MasterRenderer() {}

	inline void initialiseScreenQuad();

	inline void initialiseGBuffer();

	inline void initialiseSSAOBuffer();

	inline void initialiseScreenFramebuffer();

	inline void initialiseSkybox();

	inline void initialiseLights();

	inline void initialiseSamplers();

	void initialiseRenderer(Window* pwin, Camera& pCam);
	void initialiseFramebuffers();
	void initialiseShaders();

	void reInitialiseFramebuffers();
	void destroyFramebufferTextures();

	void setActiveCam(Camera& pCam);

	void render();

	void bakeStaticLights();

	void setResolution(int resIndex)
	{
		rC.renderResolution = glm::fvec2(validResolutionsRaw[0][resIndex], validResolutionsRaw[1][resIndex]) * rC.frameScale;
		reInitialiseFramebuffers();
	}

	void setFrameScale(float pFrameScale)
	{
		rC.frameScale = pFrameScale;
	}

	static const s32 validResolutionsRaw[2][NUM_VALID_RESOLUTIONS];
	static const glm::ivec2 getValidResolution(int pIndex)
	{
		return glm::ivec2(validResolutionsRaw[0][pIndex], validResolutionsRaw[1][pIndex]);
	}

	template <class T>
	struct Rect
	{
		Rect() {}
		~Rect() {}

		T bot, left, width, height;
	};

	//std::vector<MeshInstance> ents;

	struct MeshUseMeta
	{
		MeshUseMeta(u32 pID, MeshRenderMeta pMRM) : id(pID), renderMeta(pMRM) {}
		u32 id;
		MeshRenderMeta renderMeta;
	};

	//static const u32 maxObjects = 65536;
	//MeshGPUMeta meta[maxObjects];

	//std::vector<GLuint> 

	u32 drawCount;

	ComputeShader frustCullShader;



	//GLuint objectMetaBuffer;
	//GLuint boundsBuffer;
	//GLuint texHandleBuffer;
	//GLuint drawIndirectBuffer;
	//GLuint distsBuffer;
	//GLuint transformBuffer;
	//GLuint instanceBuffer;

	//std::unordered_map<Mesh*, std::vector<glm::fmat4>> entities;
	std::vector<PointLightData> pointLights;
	std::vector<SpotLightData> spotLights;
	std::vector<DirectLightData> directLights;

	Rect<int> viewport;

	Window* window;
	World* world;

	DefaultFramebuffer fboDefault;
	Framebuffer fboSSAOBlur;
	Framebuffer fboSSAO;
	Framebuffer fboGBuffer;
	Framebuffer fboScreen;

	GLuint vaoQuad;
	GLuint vboQuad;

	GLuint vaoQuadViewRays;
	GLuint vboQuadViewRays;

	Shader skyboxShader;
	GLuint skyboxTex;
	GLuint vaoSkybox, vboSkybox;

	GBufferShader gBufferShader;
	SAOShader ssaoShader;
	DeferredTileCullComputeShader tileCullShader;

	Shader blurShader;
	Shader shadowShader;
	Framebuffer fboLight;
	float shadScale;
	const glm::ivec2 shadRes = glm::ivec2(1000, 1000);

	LightManager lightManager;

	GLTexture2D th;
	glm::fmat4 lightView;
	Camera* activeCam;
	GLTextureCube sk;

	Sampler defaultSampler;
	Sampler postSampler;
	Sampler cubeSampler;
	Sampler textSampler;

	struct RenderConfig
	{
		glm::ivec2 renderResolution;
		float frameScale;
		float ssaoPower;
		float ssaoScale;
		bool wireFrame;
		//FOG
		//AA level
		//Tonemap settings (exposure + curve parameters)
		//Motion blur
		//DoF
		//Vignetting
		//draw AABB
	} rC;
};
