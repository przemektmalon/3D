#pragma once
#include "MathIncludes.hpp"

#define min(a,b) a > b ? b : a
#define max(a,b) a > b ? a : b

class AABounds3D
{
public:
	AABounds3D() {}
	AABounds3D(glm::fvec3 pP, glm::fvec3 pS) : p(pP), s(pS) {}
	~AABounds3D() {}

	void set(glm::fvec3 pP, glm::fvec3 pS) { p = pP; s = pS; }

	bool contains(glm::fvec3 point)
	{
		return (point.x > p.x && point.x < p.x + s.x) && (point.y > p.y && point.y < p.y + s.y) && (point.z > p.z && point.z < p.z + s.z);
	}
	bool intersects(const AABounds3D& pTarget, AABounds3D& pIntersection)
	{
		float minX1 = min(p.x, p.x + s.x);
		float maxX1 = max(p.x, p.x + s.x);
		float minY1 = min(p.y, p.y + s.y);
		float maxY1 = max(p.y, p.y + s.y);
		float minZ1 = min(p.z, p.z + s.z);
		float maxZ1 = max(p.z, p.z + s.z);

		float minX2 = min(pTarget.p.x, pTarget.p.x + pTarget.s.x);
		float maxX2 = max(pTarget.p.x, pTarget.p.x + pTarget.s.x);
		float minY2 = min(pTarget.p.y, pTarget.p.y + pTarget.s.y);
		float maxY2 = max(pTarget.p.y, pTarget.p.y + pTarget.s.y);
		float minZ2 = min(pTarget.p.z, pTarget.p.z + pTarget.s.z);
		float maxZ2 = max(pTarget.p.z, pTarget.p.z + pTarget.s.z);

		float botIntersect = max(minY1, minY2);
		float leftIntersect = max(minX1, minX2);
		float rightIntersect = min(maxX1, maxX2);
		float topIntersect = min(maxY1, maxY2);
		float zIntersect = min(maxZ1, maxZ2);
		float nzIntersect = min(maxZ1, maxZ2);

		if ((leftIntersect < rightIntersect) && (botIntersect < topIntersect) && (zIntersect < nzIntersect))
		{
			pIntersection = AABounds3D(glm::fvec3(leftIntersect, botIntersect, zIntersect), glm::fvec3(rightIntersect - leftIntersect, topIntersect - botIntersect, nzIntersect - zIntersect));
			return true;
		}
		else
		{
			pIntersection = AABounds3D(glm::fvec3(0), glm::fvec3(0));
			return false;
		}
	}

private:

	glm::fvec3 p;
	glm::fvec3 s;

};

#undef min
#undef max