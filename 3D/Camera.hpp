#pragma once
#include "Include.hpp"
#include "Shader.hpp"
#include "Time.hpp"

class Camera
{
public:
	Camera() 
	{
		yaw = 0; pitch = 0; roll = 0;
		targetYaw = 0; targetPitch = 0; targetRoll = 0;
		pos = glm::fvec3(0, 0, 0);
		targetPos = glm::fvec3(0, 0, 0);
		turnLerpFactor = 1.15;
		moveLerpFactor = 3.f;
	}
	~Camera() {}

	void initialiseProj(float pAspect, float pFOV = float(PI) / 2.5f, float pNear = 0.1f, float pFar = 1000000.f)
	{
		nearClip = pNear;
		farClip = pFar;
		fov = pFOV;
		aspect = pAspect;

		recalculateProj();
	}

	void recalculateProj()
	{
		proj = glm::perspective(fov, aspect, nearClip, farClip);
		inverseProj = glm::inverse(proj);
		calculateViewRays();
	}

	void initaliseOrtho(int sizeX, int sizeY, float pNear = 0.99f, float pFar = 100.f)
	{
		//proj = glm::ortho(-float(sizeX / 2), float(sizeX / 2), -float(sizeY / 2), float(sizeY / 2), pNear, pFar);
		proj = glm::ortho(0.f, float(sizeX), 0.f, float(sizeY), pNear, pFar);
	}

	float ler(float a, float b, float N)
	{
		float v = ((a * (N - 1)) + b) / N;
		return v;
	}

	glm::fvec3 getDirectionVector()
	{
		return glm::fvec3(glm::sin(yaw) * glm::cos(pitch), -glm::sin(pitch), -glm::cos(yaw) * glm::cos(pitch));
	}

	void calculateViewRays()
	{
		viewRays[0] = glm::fvec4(-1.f, -1.f, -1.f, 1.f);//BL
		viewRays[1] = glm::fvec4(1.f, -1.f, -1.f, 1.f);//BR
		viewRays[2] = glm::fvec4(1.f, 1.f, -1.f, 1.f);//TR
		viewRays[3] = glm::fvec4(-1.f, 1.f, -1.f, 1.f);//TL

		for (int i = 0; i < 4; ++i)
		{
			viewRays[i] = inverseProj * viewRays[i];
			viewRays[i] /= viewRays[i].w;
			viewRays[i] /= viewRays[i].z;
		}

		viewRays2[0] = glm::fvec2(viewRays[0]);
		viewRays2[1] = glm::fvec2(viewRays[1]);
		viewRays2[2] = glm::fvec2(viewRays[2]);
		viewRays2[3] = glm::fvec2(viewRays[3]);
	}

	void update(Time& dt)
	{
		targetPitch = glm::clamp(targetPitch, -float(PI) / 2.f, float(PI) / 2.f);
		pitch = ler(pitch, targetPitch, turnLerpFactor);
		yaw = ler(yaw, targetYaw, turnLerpFactor);
		
		matRoll = glm::rotate(glm::fmat4(), roll, glm::vec3(0.0f, 0.0f, 1.0f));
		matPitch = glm::rotate(glm::fmat4(), pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		matYaw = glm::rotate(glm::fmat4(), yaw, glm::vec3(0.0f, 1.0f, 0.0f));

		pos.x = ler(pos.x, targetPos.x, moveLerpFactor);
		pos.y = ler(pos.y, targetPos.y, moveLerpFactor);
		pos.z = ler(pos.z, targetPos.z, moveLerpFactor);

		glm::fmat4 translate = glm::fmat4(1.0f);
		translate = glm::translate(translate, -pos);

		qRot = glm::angleAxis(targetPitch, glm::vec3(1.0f, 0.0f, 0.0f));
		qRot *= glm::angleAxis(targetYaw, glm::vec3(0.0f, 1.0f, 0.0f));

		rotation = glm::fmat4(qRot);
		view = rotation * translate;

		projView = proj * view;
	}

	void setFOV(float pFOV);

	glm::fvec3 pos;
	glm::fvec3 targetPos;
	float yaw, pitch, roll;
	float targetYaw, targetPitch, targetRoll;
	glm::fmat4 matYaw;
	glm::fmat4 matPitch;
	glm::fmat4 matRoll;
	glm::fmat4 rotation;

	glm::quat qRot;

	glm::fmat4 proj, view, projView;
	glm::fmat4 inverseProj;

	float nearClip, farClip;
	float fov, aspect;
	float turnLerpFactor;
	float moveLerpFactor;

	glm::fvec4 viewRays[4];
	glm::fvec2 viewRays2[4];
	glm::fvec4 viewRaysDat[4];
};