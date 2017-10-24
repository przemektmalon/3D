#pragma once
#include "Renderer.hpp"

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