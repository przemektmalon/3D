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

	void initialiseProj(float pAspect, float pFOV = float(PI) / 2.5f, float pNear = 5.f, float pFar = 10000.f)
	{
		fov = pFOV;
		aspect = pAspect;
		proj = glm::perspective(fov, aspect, pNear, pFar);
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

	void update(Time& dt)
	{
		targetPitch = glm::clamp(targetPitch, -float(PI) / 2.f, float(PI) / 2.f);
		pitch = ler(pitch, targetPitch, 1.5f);
		yaw = ler(yaw, targetYaw, 1.5f);


		matRoll = glm::rotate(glm::fmat4(), roll, glm::vec3(0.0f, 0.0f, 1.0f));
		matPitch = glm::rotate(glm::fmat4(), pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		matYaw = glm::rotate(glm::fmat4(), yaw, glm::vec3(0.0f, 1.0f, 0.0f));

		pos.x = ler(pos.x, targetPos.x, 5.f);
		pos.y = ler(pos.y, targetPos.y, 5.f);
		pos.z = ler(pos.z, targetPos.z, 5.f);

		glm::fmat4 translate = glm::fmat4(1.0f);
		translate = glm::translate(translate, -pos);

		rotation = matRoll * matPitch * matYaw;
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

	glm::fmat4 proj, view, projView;

	float fov, aspect;

	//GLuint ubo;
	//GLuint uboBlockIndex;
};