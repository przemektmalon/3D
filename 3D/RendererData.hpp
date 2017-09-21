#pragma once
#include "Renderer.hpp"

#define NUM_VALID_RESOLUTIONS 8

const s32 MasterRenderer::validResolutionsRaw[2][NUM_VALID_RESOLUTIONS] =
{
	{ 1920, 1600, 1536, 1366, 1280, 1024, 960, 848 },
	{ 1080, 900,  864,  768 , 720 , 576 , 540, 480 },
};

const GLfloat quadVertices[] = {
	-1.0f,  1.0f,  0.0f, 1.0f,//TL
	1.0f,  1.0f,  1.0f, 1.0f,//TR
	1.0f, -1.0f,  1.0f, 0.0f,//BR

	1.0f, -1.0f,  1.0f, 0.0f,//BR
	-1.0f, -1.0f,  0.0f, 0.0f,//BL
	-1.0f,  1.0f,  0.0f, 1.0f//TL
};

GLfloat quadVerticesViewRays[] = {
	-1.0f,  1.0f,  0.0f, 1.0f, 999.f, 999.f,
	1.0f,  1.0f,  1.0f, 1.0f, 999.f, 999.f,
	1.0f, -1.0f,  1.0f, 0.0f, 999.f, 999.f,

	1.0f, -1.0f,  1.0f, 0.0f, 999.f, 999.f,
	-1.0f, -1.0f,  0.0f, 0.0f, 999.f, 999.f,
	-1.0f,  1.0f,  0.0f, 1.0f, 999.f, 999.f,
};