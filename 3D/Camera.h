#pragma once
#include "Include.h"
#include "Shader.h"

class Camera
{
public:
	Camera() 
	{
		//StandardShader s;

		//uboBlockIndex = glGetUniformBlockIndex(s(), "Mat");
		//GLint blockSize;
		//glGetActiveUniformBlockiv(s(), uboBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);
		//GLubyte* blockBuffer = (GLubyte*)malloc(blockSize);
		//const GLchar* names[] = { "proj" };
		//GLuint indices[1];
		//glGetUniformIndices(s(), 1, names, indices);
		//GLint offset[1];
		//glGetActiveUniformsiv(s(), 1, indices, GL_UNIFORM_OFFSET, offset);

		//memcpy(blockBuffer + offset[0], glm::value_ptr(projView), 16 * sizeof(GLfloat));

		//glGenBuffers(1, &ubo);
		//glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		//glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_STREAM_DRAW);

		//glBindBufferBase(GL_UNIFORM_BUFFER, uboBlockIndex, ubo);

		//fov = float(PI) / 2.5f;
		//aspect = float(Engine::window.getSizeX()) / float(Engine::window.getSizeY());
		//proj = glm::perspective(fov, aspect, 5.f, 10000.f);
		//proj = glm::infinitePerspective(fov, aspect, 5.f);

		yaw = 0; pitch = 0; roll = 0;
		targetYaw = 0; targetPitch = 0; targetRoll = 0;
		pos = glm::fvec3(0, 0, 0);
		targetPos = glm::fvec3(0, 0, 0);
	}
	~Camera() {}

	//void initialiseProj(float pAspect, float pFOV = float(PI) / 2.5f, float pNear = 0.1f, float pFar = 100000000000000000000000000000000000000.f)
	void initialiseProj(float pAspect, float pFOV = float(PI) / 2.5f, float pNear = 0.0000001f, float pFar = 10000.f)
	{
		fov = pFOV;
		aspect = pAspect;
		proj = glm::perspective(fov, aspect, pNear, pFar);
		inverseProj = glm::inverse(proj);
		infProj = glm::infinitePerspective(fov, aspect, pNear);

		//float tanHalfFovy = std::tan(fov / 2.f);

		//proj = glm::fmat4(0.f);
		//proj[0][0] = 1.f / (aspect * tanHalfFovy);
		//proj[1][1] = 1.f / (tanHalfFovy);
		//proj[2][2] = (pNear) / (pNear - pFar);
		//proj[2][3] = -1.f;
		//proj[3][2] = (pFar * pNear) / (pNear - pFar);
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
			//viewRays[i] = view * glm::fvec4(viewRays[i].x, viewRays[i].y, 1.f, 1.f);
		}

		viewRays2[0] = glm::fvec2(viewRays[0]);
		viewRays2[1] = glm::fvec2(viewRays[1]);
		viewRays2[2] = glm::fvec2(viewRays[2]);
		viewRays2[3] = glm::fvec2(viewRays[3]);

		//viewRaysDat = glm::fvec4(viewRays[0].x, viewRays[0].y, viewRays[2].x, viewRays[2].y);
	}

	void calculateViewRaysDat()
	{
		//glm::fvec4 p1(viewRays[0].x, viewRays[0].y, 1.f, 1.f);
		//glm::fvec4 p2(viewRays[2].x, viewRays[2].y, 1.f, 1.f);

		//p1 = view * p1;
		//p2 = view * p2;

		for (int i = 0; i < 4; ++i)
		{
			//viewRaysDat[i] = glm::normalize(glm::fvec4(viewRays[i].x, viewRays[i].y, 1.f, 1.f) * glm::transpose(view));
			viewRaysDat[i] = viewRays[i];
		}

		//viewRaysDat[0].x = p1.x; viewRaysDat[0].y = p1.y; viewRaysDat[0].z = p1.z;
		//viewRaysDat[1].x = p2.x; viewRaysDat[1].y = p2.y; viewRaysDat[1].z = p2.z;
	}

	void update(Time& dt)
	{
		targetPitch = glm::clamp(targetPitch, -float(PI) / 2.f, float(PI) / 2.f);
		float weightFactor = 1.15f;
		pitch = ler(pitch, targetPitch, weightFactor);
		yaw = ler(yaw, targetYaw, weightFactor);
		
		matRoll = glm::rotate(glm::fmat4(), roll, glm::vec3(0.0f, 0.0f, 1.0f));
		matPitch = glm::rotate(glm::fmat4(), pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		matYaw = glm::rotate(glm::fmat4(), yaw, glm::vec3(0.0f, 1.0f, 0.0f));

		pos.x = ler(pos.x, targetPos.x, 3.f);
		pos.y = ler(pos.y, targetPos.y, 3.f);
		pos.z = ler(pos.z, targetPos.z, 3.f);

		glm::fmat4 translate = glm::fmat4(1.0f);
		translate = glm::translate(translate, -pos);

		//rotation = matRoll * matPitch * matYaw;
		rotation = glm::fmat4(qRot);
		view = rotation * translate;

		projView = proj * view;
	}

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
	glm::fmat4 infProj;

	float fov, aspect;

	glm::fvec4 viewRays[4];
	glm::fvec2 viewRays2[4];
	glm::fvec4 viewRaysDat[4];

	//GLuint ubo;
	//GLuint uboBlockIndex;
};