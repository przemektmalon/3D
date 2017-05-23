#pragma once
#include "Mesh.h"
#include "Renderer.h"

class MeshBatch
{
public:
	GLint firsts[MAX_BATCH_COUNT];
	GLsizei counts[MAX_BATCH_COUNT];
	GLsizei length;

	GLfloat* data[MAX_BATCH_COUNT];
	GLint dataSizeInBytes[MAX_BATCH_COUNT];

	glm::fvec3 radii[MAX_BATCH_COUNT];

	GLuint vboID;
	GLuint vaoID;
};

class GPUMeshManager
{
public:
	GPUMeshManager() {}
	~GPUMeshManager() {}

	void pushMeshToBatch(Mesh& pMesh)
	{
		//NEW METHOD
		//Sort by Vertex Format (Material)
		//Handle multiple tri lists per material
		//Push multiple tri lists that share format at same time
		//Each tri list now has own render meta

		u32 batchID = 0;
		
		for (auto itr = pMesh.triangleListsSorted.begin(); itr != pMesh.triangleListsSorted.end(); ++itr)
		{
			auto find = solidBatches.find(itr->first);

			MeshBatch* batchPtr;

			if (find == solidBatches.end())///OPTIMIZE: INSERT HAS FIND
			{
				auto ins = solidBatches.insert(std::make_pair(itr->first, MeshBatch()));
				batchPtr = &ins.first->second;

				auto program = Engine::r->shaderStore.getShader(String<32>("gBufferPassMultiTex"));

				program->use();

				glGenVertexArrays(1, &batchPtr->vaoID);
				glBindVertexArray(batchPtr->vaoID);
				glGenBuffers(1, &batchPtr->vboID);
				glBindBuffer(GL_ARRAY_BUFFER, batchPtr->vboID);

				glBufferData(GL_ARRAY_BUFFER, MAX_BATCH_SIZE, NULL, GL_STATIC_DRAW);

				batchPtr->length = 0;
				
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

			}
			else
			{
				batchPtr = &find->second;
			}

			auto& batch = *batchPtr;

			GLsizei size = 0;
			for (int i = 0; i < batch.length; ++i)
			{
				size += batch.dataSizeInBytes[i];
			}

			for (auto itr2 = itr->second.begin(); itr2 != itr->second.end(); ++itr2)
			{
				auto prevFirst = batch.length == 0 ? 0 : batch.firsts[batch.length - 1];
				auto prevCount = batch.length == 0 ? 0 : batch.counts[batch.length - 1];

				batch.firsts[batch.length] = prevFirst + prevCount;
				batch.counts[batch.length] = (*itr2)->numVerts;
				batch.data[batch.length] = (*itr2)->data;
				batch.dataSizeInBytes[batch.length] = (*itr2)->getDataSizeInBytes();
				batch.radii[batch.length] = glm::fvec3(100.f);

				glBindBuffer(GL_ARRAY_BUFFER, batch.vboID);

				glBufferSubData(GL_ARRAY_BUFFER, size, batch.dataSizeInBytes[batch.length], batch.data[batch.length]);

				glBindBuffer(GL_ARRAY_BUFFER, 0);

				(*itr2)->renderMeta.batchPtr = &batch;
				(*itr2)->renderMeta.batchIndex = batch.length;

				size += batch.dataSizeInBytes[batch.length];
				++batch.length;
			}

			///TODO: Shadow vaos etc!

			//if (pMesh.castsShadows)
			if(itr->first == Regular)
			{
				auto find2 = solidBatches.find(DrawMode::Shadow);
				if (find2 == solidBatches.end())
				{
					auto ins = solidBatches.insert(std::make_pair(DrawMode::Shadow, MeshBatch()));
					batchPtr = &ins.first->second;

					auto program = &Engine::r->spotShadowPassShader;
					program->use();

					glGenVertexArrays(1, &batchPtr->vaoID);
					glBindVertexArray(batchPtr->vaoID);
					glGenBuffers(1, &batchPtr->vboID);
					glBindBuffer(GL_ARRAY_BUFFER, batchPtr->vboID);

					glBufferData(GL_ARRAY_BUFFER, MAX_BATCH_SIZE, NULL, GL_STATIC_DRAW);

					batchPtr->length = 0;

					auto posAttrib = glGetAttribLocation(program->getGLID(), "p");
					glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
					glEnableVertexAttribArray(posAttrib);

					glBindBuffer(GL_ARRAY_BUFFER, 0);
					glBindVertexArray(0);
				}
				else
				{
					batchPtr = &find2->second;
				}

				auto& shadowBatch = *batchPtr;

				GLsizei sizea = 0;
				for (int i = 0; i < shadowBatch.length; ++i)
				{
					sizea += shadowBatch.dataSizeInBytes[i];
				}

				for (auto itr2 = itr->second.begin(); itr2 != itr->second.end(); ++itr2)
				{
					int posSize = (*itr2)->numVerts * 3;

					float* posData = new float[posSize];

					for (int i = 0; i < (*itr2)->numVerts; ++i)
					{
						posData[(3 * i) + 0] = (*itr2)->data[(i * 8) + 0];
						posData[(3 * i) + 1] = (*itr2)->data[(i * 8) + 1];
						posData[(3 * i) + 2] = (*itr2)->data[(i * 8) + 2];
					}

					auto prevFirst = shadowBatch.length == 0 ? 0 : shadowBatch.firsts[shadowBatch.length - 1];
					auto prevCount = shadowBatch.length == 0 ? 0 : shadowBatch.counts[shadowBatch.length - 1];

					shadowBatch.firsts[shadowBatch.length] = prevFirst + prevCount;
					shadowBatch.counts[shadowBatch.length] = (*itr2)->numVerts;
					shadowBatch.data[shadowBatch.length] = posData;
					//shadowBatch.data[shadowBatch.length] = (*itr2)->data;
					shadowBatch.dataSizeInBytes[shadowBatch.length] = posSize * sizeof(float);
					//shadowBatch.dataSizeInBytes[shadowBatch.length] = (*itr2)->getDataSizeInBytes();
					shadowBatch.radii[shadowBatch.length] = glm::fvec3(100.f);

					glBindBuffer(GL_ARRAY_BUFFER, shadowBatch.vboID);

					glBufferSubData(GL_ARRAY_BUFFER, sizea, shadowBatch.dataSizeInBytes[shadowBatch.length], shadowBatch.data[shadowBatch.length]);

					glBindBuffer(GL_ARRAY_BUFFER, 0);

					(*itr2)->shadowRenderMeta.batchPtr = &shadowBatch;
					(*itr2)->shadowRenderMeta.batchIndex = shadowBatch.length;

					sizea += shadowBatch.dataSizeInBytes[shadowBatch.length];
					++shadowBatch.length;

					/*auto prevFirst = shadowBatch.length == 0 ? 0 : shadowBatch.firsts[shadowBatch.length - 1];
					auto prevCount = shadowBatch.length == 0 ? 0 : shadowBatch.counts[shadowBatch.length - 1];

					shadowBatch.firsts[shadowBatch.length] = prevFirst + prevCount;
					shadowBatch.counts[shadowBatch.length] = (*itr2)->numVerts;
					shadowBatch.data[shadowBatch.length] = (*itr2)->data;
					shadowBatch.dataSizeInBytes[shadowBatch.length] = (*itr2)->getDataSizeInBytes();
					shadowBatch.radii[shadowBatch.length] = glm::fvec3(100.f);

					glBindBuffer(GL_ARRAY_BUFFER, shadowBatch.vboID);

					glBufferSubData(GL_ARRAY_BUFFER, sizea, shadowBatch.dataSizeInBytes[shadowBatch.length], shadowBatch.data[shadowBatch.length]);

					glBindBuffer(GL_ARRAY_BUFFER, 0);

					(*itr2)->shadowRenderMeta.batchPtr = &shadowBatch;
					(*itr2)->shadowRenderMeta.batchIndex = shadowBatch.length;

					sizea += shadowBatch.dataSizeInBytes[shadowBatch.length];
					++shadowBatch.length;*/

					delete[] posData;
				}
			}
		}
	}

	void newBatch();

	std::map<DrawMode, MeshBatch> solidBatches;
};