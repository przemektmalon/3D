#include "GPUMeshManager.hpp"
#include "Engine.hpp"
#include "ShaderStore.hpp"
#include "Renderer.hpp"

void GPUMeshManager::newBatch()
{
	MeshBatch nextBatch;

	auto program = Engine::r->shaderStore.getShader(String<32>("gBufferPass"));

	program->use();

	glGenVertexArrays(1, &nextBatch.vaoID);
	glBindVertexArray(nextBatch.vaoID);
	glGenBuffers(1, &nextBatch.vboID);
	glBindBuffer(GL_ARRAY_BUFFER, nextBatch.vboID);

	glBufferData(GL_ARRAY_BUFFER, MAX_BATCH_SIZE, NULL, GL_STATIC_DRAW);

	nextBatch.length = 0;
	//nextBatch.firsts[0] = 0;
	//nextBatch.counts[0] = 0;

	auto posAttrib = glGetAttribLocation(program->getGLID(), "p");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(posAttrib);

	auto texAttrib = glGetAttribLocation(program->getGLID(), "t");
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(texAttrib);

	auto norAttrib = glGetAttribLocation(program->getGLID(), "n");
	glVertexAttribPointer(norAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(norAttrib);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//solidBatches.push_back(nextBatch);
}
