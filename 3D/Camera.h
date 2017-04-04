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

		proj = glm::perspective(float(PI) / 2.5f, float(Engine::window.sizeX) / float(Engine::window.sizeY), 1.f, 10000.f);

		yaw = 0; pitch = 0; roll = 0;
		targetYaw = 0; targetPitch = 0; targetRoll = 0;
		pos = glm::fvec3(0, 0, 0);
	}
	~Camera() {}

	float ler(float a, float b)
	{
		float N = 1.5f;
		float v = ((a * (N - 1)) + b) / N;
		return v;
	}

	void update(Time& dt)
	{
		targetPitch = glm::clamp(targetPitch, -float(PI) / 2.f, float(PI) / 2.f);
		pitch = ler(pitch, targetPitch);
		yaw = ler(yaw, targetYaw);


		matRoll = glm::rotate(glm::fmat4(), roll, glm::vec3(0.0f, 0.0f, 1.0f));
		matPitch = glm::rotate(glm::fmat4(), pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		matYaw = glm::rotate(glm::fmat4(), yaw, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::fmat4 translate = glm::fmat4(1.0f);
		translate = glm::translate(translate, -pos);

		glm::fmat4 rotate = matRoll * matPitch * matYaw;
		view = rotate * translate;

		rotate = matYaw;

		projView = proj * view;
	}

	glm::fvec3 pos;
	float yaw, pitch, roll;
	float targetYaw, targetPitch, targetRoll;
	glm::fmat4 matYaw;
	glm::fmat4 matPitch;
	glm::fmat4 matRoll;
	glm::fmat4 rotation;

	glm::fmat4 proj, view, projView;

	//GLuint ubo;
	//GLuint uboBlockIndex;
};