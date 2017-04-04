#pragma once
#include "Include.h"

struct PointLight
{
public:
	PointLight() {}
	PointLight(glm::fvec3 pos, glm::fvec3 col, float lin = 0.0001f, float quad = 0.0003f) : position(pos), colour(col), linear(lin), quadratic(quad) {}
	~PointLight() {}

	glm::fvec3 position, colour;
	float linear, quadratic;
};

struct SpotLight
{
	SpotLight() {}
	SpotLight(glm::fvec3 pos, glm::fvec3 dir, glm::fvec3 col, float inner, float outer = 0.f, float lin = 0.0001f, float quad = 0.0003f) : position(pos), direction(dir), colour(col), linear(lin), quadratic(quad), innerSpread(inner), outerSpread(outer) {
		if (outer = 0.f) { outer = inner; }
	}
	~SpotLight() {}

	glm::fvec3 position, direction, colour;
	float linear, quadratic, innerSpread, outerSpread;
};

struct DirectLight
{
	DirectLight() {}
	DirectLight(glm::fvec3 dir, glm::fvec3 col) : direction(dir), colour(col) {}
	~DirectLight() {}

	glm::fvec3 direction, colour;
};
