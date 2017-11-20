#pragma once
#include <unordered_map>
#include "Transform.hpp"
#include "SAOShader.hpp"
#include "GBufferShader.hpp"
#include "Framebuffer.hpp"
#include "BufferObjects.hpp"
#include <chrono>
#include <time.h>
#include "Lights.hpp"
#include "DeferredTileCullComputeShader.hpp"
#include "GBufferShaderMultiTex.hpp"
#include "BilateralBlurShader.hpp"
#include "FrustCullShader.hpp"
#include "PrepMultiTexShader.hpp"
#include "PointShadowPassShader.hpp"
#include "SpotShadowPassShader.hpp"
#include "TextShader.hpp"
#include "GBufferShaderBase.hpp"
#include "Sampler.hpp"
#include "ShaderStore.hpp"
#include "Shape3DShader.hpp"
#include "Shape2DShaderNoTex.hpp"
#include "TextBillboard.hpp"
#include "Window.hpp"
#include "Model.hpp"
#include "Camera.hpp"
#include "TextBillboard.hpp"

#include "GBufferShaderNonBindlessRegular.hpp"

#define NUM_VALID_RESOLUTIONS 9

class RectangleShape;
class UIConsole;

// OpenGL draw command structure for issuing indirect draw calls
typedef struct {
	u32 count;
	u32 instanceCount;
	u32 first;
	union
	{
		u32 baseInstance;
		float radius;
	};
} GLCMD;

enum DrawMode { Regular, MultiTextured, Water, Reflective, Shadow, DrawModesCount };
///TODO: Maybe add different vertex format draw modes: no-normal, etc

//Draw modes :
//Regular : Single albedo, normal and specular textures. Normal and/or specular can be 0.
//Multi-textured : Splattered textures with 4x regular textures


class MasterRenderer
{
public:
	MasterRenderer() {}
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

	void gBufferPass();
	void shadowPass();
	void ssaoPass();
	void shadingPass();
	void screenPass();

	void bakeStaticLights();

	u32 drawCount[DrawModesCount];

	irect viewport;

	Window* window;
	World* world;
	
	DefaultFramebuffer fboDefault;
	Framebuffer fboSSAOBlur;
	Framebuffer fboSSAO;
	Framebuffer fboGBuffer;
	Framebuffer fboScreen;

	Framebuffer fboLight[4];
	
	GLuint vaoQuad;
	GLuint vboQuad;

	GLuint vaoQuadViewRays;
	GLuint vboQuadViewRays;

	GLuint skyboxTex;
	GLTextureCube shadow;

	GLBufferObject shadowMatrixBuffer;

	TextBillboard* tb;

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
	Shape2DShader shape2DShader;
	Shape2DShaderNoTex shape2DShaderNoTex;
	Shape3DShader shape3DShader;
	TextShader textShader;
	
	LightManager lightManager;
	
	GLTexture2D lightPassTex;
	Camera* activeCam;

	Sampler defaultSampler;
	Sampler billboardSampler;
	Sampler postSampler;
	Sampler cubeSampler;
	Sampler textSampler;
	Sampler shadowSampler;
	Sampler shadowCubeSampler;

	// Profiling

	u64 gpuBufferTime, gBufferTime, shadowTime, ssaoTime, lightPassTime, screenTime;

	enum RenderMode { Shaded, Albedo, Normal, SSAO, Depth } renderMode;

};