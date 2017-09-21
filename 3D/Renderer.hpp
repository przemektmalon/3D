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
#include "TextBillboard.hpp"
#include "Window.hpp"
#include "Model.hpp"
#include "Camera.hpp"


#include "GBufferShaderNonBindlessRegular.hpp"

#define NUM_VALID_RESOLUTIONS 8

class RectangleShape;
class UIConsole;

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

	GLTextureArray2D texArr;

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
	Shape3DShader shape3DShader;
	TextShader textShader;
	

	GBufferShaderNonBindlessRegular gBufferShaderNonBindlessRegular;

	TextBillboard b;

	LightManager lightManager;
	
	GLTexture2D th;
	Camera* activeCam;

	Sampler defaultSampler;
	Sampler postSampler;
	Sampler cubeSampler;
	Sampler textSampler;
	Sampler shadowSampler;
	Sampler shadowCubeSampler;
	Sampler arraySampler;

	struct DrawList
	{
		GBufferShaderBase* shader;
		GLint vao;
		GLBufferObject* drawIndirect;
		struct TextureGroup
		{
			GLTexture* texture;
			u32 indirectOffset;
			GLBufferObject* textureArrayIndices;
			GLBufferObject* transforms;
		};
		std::vector<TextureGroup> textureGroups;
	};

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