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
		}

		/*u32 batchID = 0;
		for (auto itr = solidBatches.begin(); itr != solidBatches.end(); ++itr)
		{
		GLsizei size = 0;
		for (int i = 0; i < itr->length; ++i)
		{
		size += itr->counts[i] * 8 * sizeof(float);
		}

		GLsizei spaceLeft = MAX_BATCH_SIZE - size;

		if (size < spaceLeft)
		{
		auto prevFirst = itr->length == 0 ? 0 : itr->firsts[itr->length-1];
		auto prevCount = itr->length == 0 ? 0 : itr->counts[itr->length-1];
		//itr->firsts[itr->length] = itr->firsts[itr->length - 1] + itr->counts[itr->length - 1];
		itr->firsts[itr->length] = prevFirst + prevCount;
		itr->counts[itr->length] = pMesh.triangleLists[0].numVerts;
		itr->data[itr->length] = pMesh.triangleLists[0].data;
		itr->dataSizeInBytes[itr->length] = pMesh.triangleLists[0].getDataSizeInBytes();
		itr->radii[itr->length] = glm::fvec3(1000.f); //TODO: CALCULATE REAL RADIUS

		///TODO: MAPPING ?
		///TODO: Handle multiple triangle lists
		glBindBuffer(GL_ARRAY_BUFFER, solidBatches[batchID].vboID);

		s32 dataSize = 0;

		for (int i = 0; i < pMesh.triangleLists.size(); ++i)
		{
		glBufferSubData(GL_ARRAY_BUFFER, size, dataSize, pMesh.triangleLists[0].data);
		dataSize += pMesh.triangleLists[i].getDataSizeInBytes();
		}

		//auto dataSize = pMesh.triangleLists[0].getDataSizeInBytes();

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		MeshRenderMeta ret;
		ret.batchID = batchID;
		ret.batchIndex = itr->length;
		pMesh.renderMeta = ret;
		pMesh.triangleLists[0].first = itr->firsts[itr->length];

		++itr->length;

		return ret;
		}
		++batchID;
		}
		assert(0);*/
	}

	void newBatch();

	//std::vector<SolidMeshBatch> solidBatches;

	std::map<DrawMode, MeshBatch> solidBatches;

};