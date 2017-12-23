#include "GPUModelManager.hpp"
#include "Engine.hpp"
#include "ShaderStore.hpp"
#include "Renderer.hpp"
#include "Model.hpp"

void GPUModelManager::init()
{
	regularBatch.vao.create();
	regularBatch.vao.bind();

	regularBatch.vbo.create();
	regularBatch.vbo.bind();

	regularBatch.vbo.bufferData(MAX_BATCH_SIZE, NULL, GL_STATIC_DRAW);
	regularBatch.length = 0;

	regularBatch.vao.addAttrib("p", 3, GL_FLOAT);
	regularBatch.vao.addAttrib("n", 3, GL_FLOAT);
	regularBatch.vao.addAttrib("t", 2, GL_FLOAT);
	regularBatch.vao.enableFor(Engine::r->gBufferShaderTex);

	shadowVAO.create();
	shadowVAO.bind();
	shadowVAO.addAttrib("p", 3, GL_FLOAT);
	shadowVAO.overwriteTotalSize(8);
	shadowVAO.enableFor(Engine::r->pointShadowPassShader);

	aabbBatch.vao.create();
	aabbBatch.vao.bind();

	aabbBatch.vbo.create();
	aabbBatch.vbo.bind();

	aabbBatch.vbo.bufferData(MAX_BATCH_COUNT * sizeof(PhysicsObject::aabbLines), NULL, GL_STREAM_DRAW);
	aabbBatch.length = 0;

	aabbBatch.vao.addAttrib("p", 3, GL_FLOAT);
	aabbBatch.vao.enableFor(*Engine::r->shaderStore.getShader("wireframe"));
}

void GPUModelManager::pushModelToBatch(Model& pModel)
{
	for (auto itr = pModel.triLists.begin(); itr != pModel.triLists.end(); ++itr)
	{
		for (auto itr2 = itr->begin(); itr2 != itr->end(); ++itr2)
		{
			auto& batch = regularBatch;

			GLsizei size = 0;
			for (int i = 0; i < batch.length; ++i)
			{
				size += batch.dataSizeInBytes[i];
			}

			auto prevFirst = batch.length == 0 ? 0 : batch.firsts[batch.length - 1];
			auto prevCount = batch.length == 0 ? 0 : batch.counts[batch.length - 1];

			batch.firsts[batch.length] = prevFirst + prevCount;
			batch.counts[batch.length] = itr2->numVerts;
			batch.data[batch.length] = itr2->data;
			batch.dataSizeInBytes[batch.length] = itr2->getDataSizeInBytes();

			batch.vao.bind();

			batch.vbo.bind();

			batch.vbo.bufferSubData(size, batch.dataSizeInBytes[batch.length], batch.data[batch.length]);

			itr2->renderMeta.batchPtr = &batch;
			itr2->renderMeta.batchIndex = batch.length;

			size += batch.dataSizeInBytes[batch.length];
			
			++batch.length;
		}
	}
}

void GPUModelManager::pushInstanceAABB(ModelInstance& pModel)
{
	auto& batch = aabbBatch;

	auto prevFirst = batch.length == 0 ? 0 : batch.firsts[batch.length - 1];
	auto prevCount = batch.length == 0 ? 0 : batch.counts[batch.length - 1];

	batch.firsts[batch.length] = prevFirst + prevCount;
	batch.counts[batch.length] = sizeof(pModel.physicsObject->aabbLines) / sizeof(glm::fvec3);
	batch.data[batch.length] = pModel.physicsObject->aabbLines;
	batch.dataSizeInBytes[batch.length] = sizeof(pModel.physicsObject->aabbLines);

	++batch.length;
}



