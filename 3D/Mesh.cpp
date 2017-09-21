#include "Mesh.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "MeshUtility.hpp"
#include "AssetManager.hpp"

VertexFormat Material::vertexFormats[MATERIALS_COUNT];
DrawMode Material::drawModes[MATERIALS_COUNT];

void InterleavedVertexData::fromOBJMeshData(OBJMeshData * obj)
{
	for (auto itr = obj->groups.begin(); itr != obj->groups.end(); ++itr)
	{
		Group* gp = nullptr;
		auto find = materialGroups.find(itr->materialName);
		if (find == materialGroups.end())
		{
			auto ins = materialGroups.insert(std::make_pair(itr->materialName, std::list<Group>()));

			ins.first->second.push_back(Group());

			gp = &ins.first->second.back();
		}
		else
		{
			find->second.push_back(Group());

			gp = &find->second.back();
		}

		auto& g = *gp;

		g.format = obj->vData.format;
		auto find2 = obj->materials.find(itr->materialName);
		if (find2 != obj->materials.end())
		{
			g.textureName.overwrite(obj->materials.find(itr->materialName)->second);
		}
		else
		{
			g.textureName.setToChars("null");
		}

		int numVert = itr->getNumVerts();
		int vertSize = obj->vData.format.size;
		g.size = numVert * vertSize;
		g.data = new float[g.size];
		auto glvertices = g.data;

		auto posArr = (std::vector<glm::fvec3>*)(obj->vData.data[0]);
		auto norArr = (std::vector<glm::fvec3>*)(obj->vData.data[1]);
		auto uvArr = (std::vector<glm::fvec2>*)(obj->vData.data[2]);

		float* pos = (float*)posArr->data();
		float* norm = (float*)norArr->data();
		float* uv = (float*)uvArr->data();

		int count = -1;
		for (auto itr2 = itr->indices.begin(); itr2 != itr->indices.end(); itr2 += 3)
		{
			int index = 3 * (*(itr2)-1);
			glvertices[++count] = pos[index];
			glvertices[++count] = pos[index + 1];
			glvertices[++count] = pos[index + 2];

			index = 2 * (*(itr2 + 1) - 1);
			glvertices[++count] = uv[index];
			glvertices[++count] = uv[index + 1];

			index = 3 * (*(itr2 + 2) - 1);
			glvertices[++count] = norm[index];
			glvertices[++count] = norm[index + 1];
			glvertices[++count] = norm[index + 2];
		}
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

		if (tl.material.matID == PNUU_TT_SS_NN)
		{
			tl.material.albedo[0] = Engine::assets.get2DTex("ter");
			tl.material.normal[0] = Engine::assets.get2DTex("terN");
			tl.material.specular[0] = Engine::assets.get2DTex("terS");
		}

		s32 dataSize;
		ifs.read((char*)&dataSize, sizeof(dataSize));
		tl.data = new float[dataSize];
		ifs.read((char*)tl.data, dataSize);
		tl.first = 0;

		triangleLists.push_back(tl);
	}
}

void Mesh::loadBinV11()
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

		String<32> texName[4], norName[4], specName[4], alphaName;

		for (int i = 0; i < 4; ++i)
		{
			ifs.read(texName[i].getString(), texName[i].getCapacity());
			texName[i].determineLength();
			tl.material.albedo[i] = Engine::assets.get2DTex(texName[i]);
		}

		for (int i = 0; i < 4; ++i)
		{
			ifs.read(specName[i].getString(), specName[i].getCapacity());
			specName[i].determineLength();
			tl.material.specular[i] = Engine::assets.get2DTex(specName[i]);
		}

		for (int i = 0; i < 4; ++i)
		{
			ifs.read(norName[i].getString(), norName[i].getCapacity());
			norName[i].determineLength();
			tl.material.normal[i] = Engine::assets.get2DTex(norName[i]);
		}

		ifs.read(alphaName.getString(), alphaName.getCapacity());
		alphaName.determineLength();
		tl.material.alpha = Engine::assets.get2DTex(alphaName);

		ifs.read((char*)&tl.material.alphaScale, sizeof(float));

		s32 dataSize;
		ifs.read((char*)&dataSize, sizeof(dataSize));
		tl.data = new float[dataSize];
		ifs.read((char*)tl.data, dataSize);
		tl.first = 0;

		triangleLists.push_back(tl);
	}
	sortTriangleLists();
}

void Mesh::loadBinV12()
{
	//New method using my File class, much cleaner!

	char major, minor;

	File file;
	file.open(diskPath, File::binary | File::in);
	file.read(major);
	file.read(minor);
	file.readStr(name);
	s32 triangleListsSize;
	file.read(triangleListsSize);
	for (auto i = 0; i < triangleListsSize; ++i)
	{
		TriangleList tl;
		file.read(tl.material.matID);
		file.read(tl.numVerts);

		String<32> texName[4], norName[4], specName[4], alphaName;

		for (int i = 0; i < 4; ++i)
		{
			file.readStr(texName[i], true);
			tl.material.albedo[i] = Engine::assets.get2DTex(texName[i]);
		}

		for (int i = 0; i < 4; ++i)
		{
			file.readStr(norName[i], true);
			tl.material.specular[i] = Engine::assets.get2DTex(norName[i]);
		}

		for (int i = 0; i < 4; ++i)
		{
			file.readStr(specName[i], true);
			tl.material.normal[i] = Engine::assets.get2DTex(specName[i]);
		}

		file.readStr(alphaName, true);
		tl.material.alpha = Engine::assets.get2DTex(alphaName);

		file.read(tl.material.alphaScale);
		s32 dataSize;
		file.read(dataSize);
		tl.data = new float[dataSize];
		file.read(tl.data, dataSize);
		tl.first = 0;

		triangleLists.push_back(tl);
	}

	file.read(castsShadows);
	sortTriangleLists();

	/*std::ifstream ifs(diskPath.getString(), std::ios_base::binary);
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

		String<32> texName[4], norName[4], specName[4], alphaName;

		for (int i = 0; i < 4; ++i)
		{
			ifs.read(texName[i].getString(), texName[i].getCapacity());
			texName[i].determineLength();
			tl.material.albedo[i] = Engine::assets.get2DTex(texName[i]);
		}

		for (int i = 0; i < 4; ++i)
		{
			ifs.read(specName[i].getString(), specName[i].getCapacity());
			specName[i].determineLength();
			tl.material.specular[i] = Engine::assets.get2DTex(specName[i]);
		}

		for (int i = 0; i < 4; ++i)
		{
			ifs.read(norName[i].getString(), norName[i].getCapacity());
			norName[i].determineLength();
			tl.material.normal[i] = Engine::assets.get2DTex(norName[i]);
		}

		ifs.read(alphaName.getString(), alphaName.getCapacity());
		alphaName.determineLength();
		tl.material.alpha = Engine::assets.get2DTex(alphaName);

		ifs.read((char*)&tl.material.alphaScale, sizeof(float));

		s32 dataSize;
		ifs.read((char*)&dataSize, sizeof(dataSize));
		tl.data = new float[dataSize];
		ifs.read((char*)tl.data, dataSize);
		tl.first = 0;

		triangleLists.push_back(tl);
	}

	ifs.read((char*)&castsShadows, sizeof(bool));
	sortTriangleLists();*/
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

Material::Material()
{
	//nullTextures(Engine::assets.get2DTex("null"));
}

void Material::initDrawModes()
{
	drawModes[PNUU_T_S_N] = DrawMode::Regular;
	drawModes[PNUU_TT_SS_NN] = DrawMode::MultiTextured;
	drawModes[PNUU_TTT_SSS_NNN] = DrawMode::MultiTextured;
	//drawModes[PNUU_TT_SS_NN] = DrawMode::MultiTextured;
}

DrawMode Material::getDrawMode(MaterialID matID)
{
	return drawModes[matID];
}

DrawMode Material::getDrawMode()
{
	return drawModes[matID];
}