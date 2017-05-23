#pragma once
#include <unordered_map>
#include "Mesh.h"
#include "Transform.h"
#include "SAOShader.h"
#include "GBufferShader.h"
#include "Framebuffer.h"
#include "BufferObjects.h"
#include <chrono>
#include <time.h>
#include "Lights.h"
#include "DeferredTileCullComputeShader.h"
#include "GBufferShaderMultiTex.h"
#include "BilateralBlurShader.h"
#include "FrustCullShader.h"
#include "PrepMultiTexShader.h"
#include "PointShadowPassShader.h"
#include "SpotShadowPassShader.h"
#include "Sampler.h"
#include "ShaderStore.h"

#define NUM_VALID_RESOLUTIONS 8

class RectangleShape;
class UIConsole;

typedef struct {
	u32 count;
	u32 instanceCount;
	u32 first;
	//u32 baseInstance;
	float radius;
} GLCMD;

enum DrawMode { Regular, MultiTextured, Water, Reflective, Shadow, DrawModesCount };
///TODO: Maybe add different vertex format draw modes: no-normal, etc

class MasterRenderer
{
public:
	MasterRenderer()// : shadScale(1.f) 
	{
		config.ssaoPower = (1.f);
		config.ssaoScale = (1.f);
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

	void cameraProjUpdated();

	void render();

	void bakeStaticLights();

	void setResolution(int resIndex)
	{
		config.renderResolution = glm::fvec2(validResolutionsRaw[0][resIndex], validResolutionsRaw[1][resIndex]) * config.frameScale;
		reInitialiseFramebuffers();
	}

	void setFrameScale(float pFrameScale)
	{
		config.frameScale = pFrameScale;
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

		T top, left, width, height;
	};

	struct MeshUseMeta
	{
		MeshUseMeta(u32 pID, MeshRenderMeta pMRM) : id(pID), renderMeta(pMRM) {}
		u32 id;
		MeshRenderMeta renderMeta;
	};


	u32 drawCount[DrawModesCount];

	Rect<int> viewport;

	Window* window;
	World* world;
	
	DefaultFramebuffer fboDefault;
	Framebuffer fboSSAOBlur;
	Framebuffer fboSSAO;
	Framebuffer fboGBuffer;
	Framebuffer fboScreen;

	s32 shadowResolutions[4] = { 256,512,1024,2048 };

	Framebuffer fboLight[4];
	
	GLuint vaoQuad;
	GLuint vboQuad;

	GLuint vaoQuadViewRays;
	GLuint vboQuadViewRays;

	GLuint skyboxTex;
	GLTextureCube shadow;

	GLBufferObject shadowMatrixBuffer;

	//Shaders
	ShaderStore shaderStore;

	GBufferShader gBufferShader;
	GBufferShaderMultiTex gBufferShaderMultiTex;
	DeferredTileCullComputeShader tileCullShader;
	BilateralBlurShader bilatBlurShader;
	FrustCullShader frustCullShader;
	PrepMultiTexShader prepMultiTexShader;
	SAOShader ssaoShader;
	SpotShadowPassShader spotShadowPassShader;
	PointShadowPassShader pointShadowPassShader;

	//Shader shadowShader;
	//Framebuffer fboLight;
	//float shadScale;
	//const glm::ivec2 shadRes = glm::ivec2(1000,1000);

	LightManager lightManager;
	
	GLTexture2D th;
	//glm::fmat4 lightView;
	Camera* activeCam;
	//GLTextureCube sk;

	Sampler defaultSampler;
	Sampler postSampler;
	Sampler cubeSampler;
	Sampler textSampler;
	Sampler shadowSampler;
	Sampler shadowCubeSampler;

	struct RenderConfig
	{
		glm::ivec2 renderResolution;
		float frameScale;
		float ssaoPower;
		float ssaoScale;
		bool drawWireFrame;
		bool drawTextBounds;
		//FOG
		//AA level
		//Tonemap settings (exposure + curve parameters)
		//Motion blur
		//DoF
		//Vignetting
		//draw AABB
	} config;
};