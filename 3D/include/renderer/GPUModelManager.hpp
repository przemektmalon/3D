#pragma once
#include "GLIncludes.hpp"
#include "BufferObjects.hpp"
#include "VertexArray.hpp"

class Model;
class ModelInstance;

#define MAX_BATCH_COUNT 512
#define MAX_BATCH_SIZE 1024*1024*128

class DrawBatch
{
public:
	GLint firsts[MAX_BATCH_COUNT];
	GLsizei counts[MAX_BATCH_COUNT];
	GLsizei length;

	GLfloat* data[MAX_BATCH_COUNT];
	GLint dataSizeInBytes[MAX_BATCH_COUNT];

	VBO vbo;
	VAO vao;
};

class GPUModelManager
{
public:
	GPUModelManager() {}
	~GPUModelManager() {}

	void init();

	void pushModelToBatch(Model& pModel);
	void pushInstanceAABB(ModelInstance& pModel);

	DrawBatch regularBatch;
	DrawBatch aabbBatch;
	VAO shadowVAO;
};