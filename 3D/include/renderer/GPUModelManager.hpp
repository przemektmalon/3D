#pragma once
#include "GLIncludes.hpp"

class Model;

#define MAX_BATCH_COUNT 512
#define MAX_BATCH_SIZE 1024*1024*128

class MeshBatch
{
public:
	GLint firsts[MAX_BATCH_COUNT];
	GLsizei counts[MAX_BATCH_COUNT];
	GLsizei length;

	GLfloat* data[MAX_BATCH_COUNT];
	GLint dataSizeInBytes[MAX_BATCH_COUNT];

	GLuint vboID;
	GLuint vaoID;
};

class GPUModelManager
{
public:
	GPUModelManager() {}
	~GPUModelManager() {}

	void init();

	void pushModelToBatch(Model& pModel);

	void newBatch();

	MeshBatch regularBatch;
	GLuint shadowVAO;
};