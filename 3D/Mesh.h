#pragma once
#include "Include.h"
#include <string>
#include <fstream>
#include "Shader.h"
#include "Transform.h"
#include "Engine.h"
#include "Asset.h"
#include "SceneGraph.h"

#include "Texture.h"

struct ObjectData
{
	std::vector<float> verts;
	std::vector<float> texCoords;
	std::vector<float> normals;
	std::vector<int> indices;
	s32 numVert;
	s32 numTris;
};

struct InterlacedObjectData
{
	float* interlacedData;
	s32 size;
};

struct MeshRenderMeta
{
	u32 batchID;
	u32 batchIndex;
};

class Mesh : public Asset
{
public:
	Mesh() {}
	Mesh(String128& pPath, String32& pName) : Asset(pPath, pName) {}
	~Mesh() {}

	void load()
	{
		loadBinary();
	}

	/*void load(std::string objPath)
	{
	std::ifstream ifs(objPath);

	if (ifs.bad())
	{
	std::cout << "Error loading " << objPath << std::endl;
	return;
	}

	if (!ifs.good())
	{
	std::cout << "Error loading " << objPath << std::endl;
	return;
	}

	if (!ifs.is_open())
	{
	std::cout << "Error loading " << objPath << std::endl;
	return;
	}

	while (!ifs.eof())
	{
	std::string type;
	std::getline(ifs, type, ' ');
	if (type == "v")
	{
	std::string xstr, ystr, zstr;
	std::getline(ifs, xstr, ' ');
	std::getline(ifs, ystr, ' ');
	std::getline(ifs, zstr, '\n');
	data.verts.push_back(std::stof(xstr));
	data.verts.push_back(std::stof(ystr));
	data.verts.push_back(std::stof(zstr));
	}
	else if (type == "vt")
	{
	std::string ustr, tstr;
	std::getline(ifs, ustr, ' ');
	std::getline(ifs, tstr, '\n');
	data.texCoords.push_back(std::stof(ustr));
	data.texCoords.push_back(std::stof(tstr));
	}
	else if (type == "vn")
	{
	std::string xstr, ystr, zstr;
	std::getline(ifs, xstr, ' ');
	std::getline(ifs, ystr, ' ');
	std::getline(ifs, zstr, '\n');
	data.normals.push_back(std::stof(xstr));
	data.normals.push_back(std::stof(ystr));
	data.normals.push_back(std::stof(zstr));
	}
	else if (type == "f")
	{
	std::string xstr, ystr, zstr;
	std::getline(ifs, xstr, '/');
	std::getline(ifs, ystr, '/');
	std::getline(ifs, zstr, ' ');
	data.indices.push_back(abs(std::stoi(xstr)));
	data.indices.push_back(abs(std::stoi(ystr)));
	data.indices.push_back(abs(std::stoi(zstr)));
	std::getline(ifs, xstr, '/');
	std::getline(ifs, ystr, '/');
	std::getline(ifs, zstr, ' ');
	data.indices.push_back(abs(std::stoi(xstr)));
	data.indices.push_back(abs(std::stoi(ystr)));
	data.indices.push_back(abs(std::stoi(zstr)));
	std::getline(ifs, xstr, '/');
	std::getline(ifs, ystr, '/');
	std::getline(ifs, zstr, '\n');
	data.indices.push_back(abs(std::stoi(xstr)));
	data.indices.push_back(abs(std::stoi(ystr)));
	data.indices.push_back(abs(std::stoi(zstr)));

	/*std::string xstr, ystr, zstr;
	std::getline(ifs, xstr, '/');
	std::getline(ifs, ystr, '/');
	std::getline(ifs, zstr, ' ');
	data.indices.push_back(abs(std::stoi(xstr)));
	data.indices.push_back(0);
	data.indices.push_back(abs(std::stoi(zstr)));
	std::getline(ifs, xstr, '/');
	std::getline(ifs, ystr, '/');
	std::getline(ifs, zstr, ' ');
	data.indices.push_back(abs(std::stoi(xstr)));
	data.indices.push_back(0);
	data.indices.push_back(abs(std::stoi(zstr)));
	std::getline(ifs, xstr, '/');
	std::getline(ifs, ystr, '/');
	std::getline(ifs, zstr, '\n');
	data.indices.push_back(abs(std::stoi(xstr)));
	data.indices.push_back(0);
	data.indices.push_back(abs(std::stoi(zstr)));///////////
	}
	else
	{
	std::getline(ifs, type, '\n');
	}
	}

	data.numVert = data.indices.size() / 3;
	data.numTris = data.numVert / 3;

	int numVert = data.numVert;
	int vertSize = 8;
	float* glvertices = new float[numVert * vertSize];

	int count = -1;
	for (auto itr = data.indices.begin(); itr != data.indices.end(); itr += 3)
	{
	int index = 3 * (*(itr)-1);
	glvertices[++count] = data.verts[index];
	glvertices[++count] = data.verts[index + 1];
	glvertices[++count] = data.verts[index + 2];

	index = 2 * (*(itr + 1) - 1);
	glvertices[++count] = data.texCoords[index];
	glvertices[++count] = data.texCoords[index + 1];

	index = 3 * (*(itr + 2) - 1);
	glvertices[++count] = data.normals[index];
	glvertices[++count] = data.normals[index + 1];
	glvertices[++count] = data.normals[index + 2];
	}

	intData.interlacedData = glvertices;
	intData.size = numVert * vertSize;

	std::cout << intData.size << std::endl;
	std::cout << data.numVert << std::endl;
	std::cout << data.numTris << std::endl;

	glUseProgram(Engine::gPassShader());

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, intData.size * sizeof(GLfloat), intData.interlacedData, GL_STATIC_DRAW);

	auto posAttrib = glGetAttribLocation(Engine::gPassShader(), "p");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(posAttrib);

	auto texAttrib = glGetAttribLocation(Engine::gPassShader(), "t");
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(texAttrib);

	auto norAttrib = glGetAttribLocation(Engine::gPassShader(), "n");
	glVertexAttribPointer(norAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(norAttrib);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	}*/

	void loadBinary()
	{
		std::ifstream ifs(diskPath.getString(), std::ios_base::binary);
		s32 size;
		float* glVerts;
		ifs.read((char*)&size, sizeof(size));
		intData.interlacedData = (new float[size]);
		ifs.read((char*)intData.interlacedData, sizeof(GLfloat) * size);
		intData.size = size;
		data.numVert = size / 8;
		data.numTris = data.numVert / 3;
	}

	void saveBinary(std::string binPath)
	{
		std::ofstream ofs(binPath, std::ios_base::binary);
		ofs.write((char*)&intData.size, sizeof(intData.size));
		std::cout << intData.size << std::endl;
		ofs.write((char*)intData.interlacedData, intData.size * sizeof(float));
	}

	ObjectData data;
	InterlacedObjectData intData;
	MeshRenderMeta renderMeta;
};

struct MeshLoadMeta
{
	String128 path;
	String32 name;
};

struct MeshInstanceMeta
{
	MeshRenderMeta renderMeta;
	SGNode* nodePtr;
};

struct MeshGPUMeta
{
	MeshGPUMeta() {}
	union
	{
		glm::uvec4 cmds;
		struct
		{
			glm::uvec3 cmdss;
			float radius;
		};
	};
	glm::uvec4 texHandleMatID;
	glm::uvec4 normalBumpMap;
};

class MeshInstance
{
public:
	MeshInstance() {}
	~MeshInstance() {}

	//private:
	//u32 meshID;
	u32 instanceID;
	MeshRenderMeta renderMeta;
	SGNode* sgNode;
	GLTexture2DMip texx;
	GLTexture2DMip specTex;
	GLTexture2DMip normalTex;
	GLTexture2DMip bumpTex;
};

#define MAX_BATCH_COUNT 512
#define MAX_BATCH_SIZE 1024*1024*64

struct MeshCPUMeta
{
	MeshCPUMeta(const char* pPath, const char* pName)
	{
		//strcpy_s(loadMeta.path, pPath);
		//strcpy_s(loadMeta.name, pName);
		loadMeta.path.setToChars(pPath);
		loadMeta.name.setToChars(pName);
	}
	MeshLoadMeta loadMeta;
	MeshRenderMeta renderMeta;
};

class GPUMeshManager
{
public:
	GPUMeshManager() {}
	~GPUMeshManager() {}

	//MeshRenderMeta loadMesh(char* pPath)
	//{
	//	InterlacedObjectData intData;
	//	std::ifstream ifs(pPath, std::ios_base::binary);
	//	s32 size;
	//	float* glVerts;
	//	ifs.read((char*)&size, sizeof(size));
	//	intData.interlacedData = (new float[size]);
	//	ifs.read((char*)intData.interlacedData, sizeof(GLfloat) * size);
	//	intData.size = size;

	//	//data.numVert = size / 8;
	//	//data.numTris = data.numVert / 3;

	//	return pushMeshToBatch(intData.interlacedData, size, intData.size * sizeof(float), intData.size / (8), 10000);
	//}

	//MeshRenderMeta pushMeshToBatch(float* pData, s32 pDataSizeInBytes, s32 pMeshSize, s32 pVertCount, float pRadius)
	MeshRenderMeta pushMeshToBatch(Mesh& pMesh)
	{
		u32 batchID = 0;
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
				auto prevFirst = itr->length == 0 ? 0 : itr->firsts[itr->length - 1];
				auto prevCount = itr->length == 0 ? 0 : itr->counts[itr->length - 1];
				//itr->firsts[itr->length] = itr->firsts[itr->length - 1] + itr->counts[itr->length - 1];
				itr->firsts[itr->length] = prevFirst + prevCount;
				itr->counts[itr->length] = pMesh.intData.size / 8;
				itr->data[itr->length] = pMesh.intData.interlacedData;
				itr->dataSizeInBytes[itr->length] = pMesh.intData.size * sizeof(float);
				itr->radii[itr->length] = glm::fvec3(1000.f); //TODO: CALCULATE REAL RADIUS

															  //TODO: MAPPING ?
				glBindBuffer(GL_ARRAY_BUFFER, solidBatches[batchID].vboID);
				glBufferSubData(GL_ARRAY_BUFFER, size, pMesh.intData.size * sizeof(float), pMesh.intData.interlacedData);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				MeshRenderMeta ret;
				ret.batchID = batchID;
				ret.batchIndex = itr->length;
				pMesh.renderMeta = ret;


				++itr->length;

				return ret;
			}
			++batchID;
		}
		assert(0);
	}

	void newBatch()
	{
		SolidMeshBatch nextBatch;

		glUseProgram(Engine::gPassShader());

		glGenVertexArrays(1, &nextBatch.vaoID);
		glBindVertexArray(nextBatch.vaoID);
		glGenBuffers(1, &nextBatch.vboID);
		glBindBuffer(GL_ARRAY_BUFFER, nextBatch.vboID);

		glBufferData(GL_ARRAY_BUFFER, MAX_BATCH_SIZE, NULL, GL_STATIC_DRAW);

		nextBatch.length = 0;
		//nextBatch.firsts[0] = 0;
		//nextBatch.counts[0] = 0;

		auto posAttrib = glGetAttribLocation(Engine::gPassShader(), "p");
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(posAttrib);

		auto texAttrib = glGetAttribLocation(Engine::gPassShader(), "t");
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(texAttrib);

		auto norAttrib = glGetAttribLocation(Engine::gPassShader(), "n");
		glVertexAttribPointer(norAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
		glEnableVertexAttribArray(norAttrib);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		solidBatches.push_back(nextBatch);
	}

private:
	class SolidMeshBatch
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

public:
	std::vector<SolidMeshBatch> solidBatches;



};