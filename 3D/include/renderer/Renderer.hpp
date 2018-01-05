#pragma once
#include <unordered_map>
#include <chrono>
#include <time.h>

#include "Framebuffer.hpp"
#include "BufferObjects.hpp"
#include "Lights.hpp"
#include "Sampler.hpp"
#include "Rect.hpp"
#include "DrawModes.hpp"
#include "VertexArray.hpp"

#include "SAOShader.hpp"
#include "GBufferShaderTex.hpp"
#include "GBufferShaderClay.hpp"
#include "DeferredTileCullComputeShader.hpp"
#include "GBufferShaderMultiTex.hpp"
#include "BilateralBlurShader.hpp"
#include "FrustCullShader.hpp"
#include "PrepMultiTexShader.hpp"
#include "PointShadowPassShader.hpp"
#include "SpotShadowPassShader.hpp"
#include "TextShader.hpp"
#include "ShaderStore.hpp"
#include "Shape3DShader.hpp"
#include "Shape2DShaderNoTex.hpp"
#include "Shape2DShader.hpp"

#define NUM_VALID_RESOLUTIONS 9

class Window;
class World;
class Camera;
class RectangleShape;
class UIConsole;

// OpenGL draw command structure for issuing indirect draw calls
typedef struct {
	u32 count;
	u32 instanceCount;
	u32 first;
	u32 baseInstance;
} GLCMD;

class Renderer
{
public:
	Window* window;
	World* world;

	// Framebuffers
	FBO fboSSAOBlur;
	FBO fboSSAO;
	FBO fboGBuffer;
	FBO fboScreen;
	FBO fboShadow;

	// Vertex storage for rendering to window
	VAO vaoQuad;
	VBO vboQuad;
	VAO vaoQuadViewRays;
	VBO vboQuadViewRays;

	// Shaders
	ShaderStore shaderStore;
	GBufferShaderTex gBufferShaderTex;
	GBufferShaderClay gBufferShaderClay;
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

	// Light stuff
	LightManager lightManager;
	GLTexture2D lightPassTex;
	GLBufferObject shadowMatrixBuffer;
	glm::ivec2 csmViewports[3];

	// Camera
	Camera* activeCam;
	irect viewport;

	// Samplers
	Sampler defaultSampler;
	Sampler billboardSampler;
	Sampler postSampler;
	Sampler cubeSampler;
	Sampler textSampler;
	Sampler shadowSampler;
	Sampler shadowCubeSampler;

	u32 drawCount[DrawModesCount];

	enum RenderMode { Shaded, Albedo, Normal, SSAO, Depth } renderMode;

	Renderer() : renderMode(Shaded) { drawCount[0] = 0; drawCount[1] = 0; }
	~Renderer() {}

	inline void initialiseScreenQuad();

	inline void initialiseGBuffer();

	inline void initialiseSSAOBuffer();

	inline void initialiseScreenFramebuffer();

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

	/// TODO: this
	// void bakeStaticLights();
};