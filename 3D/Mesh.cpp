#include "Mesh.h"
#include "Engine.h"
#include "Renderer.h"
#include "MeshUtility.h"
#include "AssetManager.h"

VertexFormat Material::vertexFormats[MATERIALS_COUNT];

void InterleavedVertexData::fromOBJMeshData(OBJMeshData * obj)
{
	format = obj->vData.format;

	int numVert = obj->getNumVerts();
	int vertSize = obj->vData.format.size;
	size = numVert * vertSize;
	//float* glvertices = new float[size];
	data = new float[size];
	auto glvertices = data;

	auto posArr = (std::vector<glm::fvec3>*)(obj->vData.data[0]);
	auto norArr = (std::vector<glm::fvec3>*)(obj->vData.data[1]);
	auto uvArr = (std::vector<glm::fvec2>*)(obj->vData.data[2]);

	float* pos = (float*)posArr->data();
	float* norm = (float*)norArr->data();
	float* uv = (float*)uvArr->data();

	//float* pos = (float*)((std::vector<glm::fvec3>*)(obj->vData.data))->data();
	//float* norm = (float*)((std::vector<glm::fvec3>*)(obj->vData.data + sizeof(int*)))->data();
	//float* uv = (float*)((std::vector<glm::fvec3>*)(obj->vData.data + (2 * sizeof(int*))))->data();

	int count = -1;
	for (auto itr = obj->indices.begin(); itr != obj->indices.end(); itr += 3)
	{
		int index = 3 * (*(itr)-1);
		glvertices[++count] = pos[index];
		glvertices[++count] = pos[index + 1];
		glvertices[++count] = pos[index + 2];

		//std::string s = std::to_string(pos[index]) + "," + std::to_string(pos[index + 1]) + "," + std::to_string(pos[index + 2]) + "\n";

		//OutputDebugStringA(s.c_str());

		index = 2 * (*(itr + 1) - 1);
		glvertices[++count] = uv[index];
		glvertices[++count] = uv[index + 1];

		index = 3 * (*(itr + 2) - 1);
		glvertices[++count] = norm[index];
		glvertices[++count] = norm[index + 1];
		glvertices[++count] = norm[index + 2];
	}
}

void Mesh::loadBinV10()
{
	std::ifstream ifs(diskPath.getString(), std::ios_base::binary);
	char major, minor;
	ifs.read(&major, sizeof(major));
	ifs.read(&minor, sizeof(minor));
	ifs.read(name.getString(), name.getCapacity());
	s32 numTriLists;
	ifs.read((char*)&numTriLists, sizeof(numTriLists));
	for (auto i = 0; i < numTriLists; ++i)
	{
		TriangleList tl;
		ifs.read((char*)&tl.material.matID, sizeof(tl.material.matID));
		ifs.read((char*)&tl.numVerts, sizeof(tl.numVerts));

		String<32> texName, norName, bumpName, specName;

		ifs.read(texName.getString(), texName.getCapacity());
		texName.determineLength();

		norName.overwrite(texName);
		//bumpName.overwrite(texName);
		specName.overwrite(texName);

		norName.insertAt(texName.getLength(), String<2>("N"));
		//bumpName.insertAt(texName.getLength(), String<2>("B"));
		specName.insertAt(texName.getLength(), String<2>("S"));

		tl.material.albedo[0] = Engine::assets.get2DTex(texName);
		tl.material.normal[0] = Engine::assets.get2DTex(norName);
		tl.material.specular[0] = Engine::assets.get2DTex(specName);

		///TODO: Fix dataSize!
		s32 dataSize;
		ifs.read((char*)&dataSize, sizeof(dataSize));
		tl.data = new float[dataSize];
		ifs.read((char*)tl.data, dataSize);
		tl.first = 0;

		triangleLists.push_back(tl);
	}
}

void VertexFormat::defineGLVertexAttribs(GLuint vao)
{
	auto program = Engine::r->shaderStore.getShader(String<32>("gBufferPass"));

	program->use();

	glBindVertexArray(vao);

	auto posAttrib = glGetAttribLocation(program->getGLID(), "p");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(posAttrib);

	auto texAttrib = glGetAttribLocation(program->getGLID(), "t");
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(texAttrib);

	auto norAttrib = glGetAttribLocation(program->getGLID(), "n");
	glVertexAttribPointer(norAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(norAttrib);

	glBindVertexArray(0);
}
