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
#include "BilateralBlurShader.h"
#include "FrustCullShader.h"
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

class MasterRenderer
{
public:
	MasterRenderer()// : shadScale(1.f) 
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

	void cameraProjUpdated();

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

		T top, left, width, height;
	};

	struct MeshUseMeta
	{
		MeshUseMeta(u32 pID, MeshRenderMeta pMRM) : id(pID), renderMeta(pMRM) {}
		u32 id;
		MeshRenderMeta renderMeta;
	};


	u32 drawCount;

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

	//Shaders
	ShaderStore shaderStore;

	GBufferShader gBufferShader;
	DeferredTileCullComputeShader tileCullShader;
	BilateralBlurShader bilatBlurShader;
	FrustCullShader frustCullShader;
	SAOShader ssaoShader;

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
