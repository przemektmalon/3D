
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <io.h>
#include <ios>
#include "MeshUtility.h"
#include "C:/Programming/Libs/GLM/glm/common.hpp"
#include "Engine.h"

const int VertexAttrib::typeSizes[VertexAttrib::AttribTypesCount] = {
	8, 12, 16,
	16, 24, 32
};

const String16 VertexAttrib::typeNames[VertexAttrib::AttribTypesCount] = {
	"ff", "fff", "ffff",
	"dd", "ddd", "dddd"
};

/*struct ObjectData
{
	std::vector<float> verts;
	std::vector<float> texCoords;
	std::vector<float> normals;
	std::vector<int> indices;
	int numVert;
	int numTris;
};

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		return 1;
	}

	char* ppath = argv[1];
	int ppathLength = 0;

	std::cout << argc << std::endl;

	for (int i = 0; i < argc; ++i)
	{
		ppathLength = 0;
		ppath = argv[i];
		while (*ppath != '\0')
		{
			std::cout << *ppath;
			ppathLength++;
			ppath++;
		}
		std::cout << std::endl;
	}

	return 0;
}*/

void OBJMeshData::loadMY(StringGeneric& pPath)
{

	char* data = nullptr;

	std::ifstream ifs(pPath.getString(), std::ios_base::binary);

	std::filebuf* pbuf = ifs.rdbuf();

	std::size_t size = pbuf->pubseekoff(0, ifs.end, ifs.in);
	pbuf->pubseekpos(0, ifs.in);

	data = new char[size];

	pbuf->sgetn(data, size);
	ifs.close();

	String<32> name;

	s32 numTriLists;

	//Extract header info
	{
		name.setToChars(data, data + 32);
		numTriLists = ((int*)data)[32];
	}


	//Extract format
	{
		s32 delimIndex;
		String<4> attribStr;
		VertexFormat format;
		String<64> formatStr;

		for (int i = 0; i < 12; ++i)
		{
			delimIndex = formatStr.find(' ');
			if (delimIndex == -1)
			{
				delimIndex = formatStr.find('\0');
			}
			formatStr.take(delimIndex, attribStr);
			formatStr.drop(delimIndex + 1);

			if (attribStr == String<5>("ffff"))
			{
				format.attribs[i].type = VertexAttrib::flt4;
			}
			else if (attribStr == String<4>("fff"))
			{
				format.attribs[i].type = VertexAttrib::flt3;
			}
			else if (attribStr == String<3>("ff"))
			{
				format.attribs[i].type = VertexAttrib::flt2;
			}
			else if (attribStr == String<5>("dddd"))
			{
				format.attribs[i].type = VertexAttrib::dbl4;
			}
			else if (attribStr == String<4>("ddd"))
			{
				format.attribs[i].type = VertexAttrib::dbl3;
			}
			else if (attribStr == String<3>("dd"))
			{
				format.attribs[i].type = VertexAttrib::dbl2;
			}

			if (formatStr.getLength() < 2)
			{
				break;
			}
		}
	}
}

void OBJMeshData::load(StringGeneric & pPath)
{
	std::ifstream ifs(pPath.getString());

	if (ifs.bad())
	{
		std::cout << "Error loading " << pPath.getString() << std::endl;
	}

	if (!ifs.good())
	{
		std::cout << "Error loading " << pPath.getString() << std::endl;
	}

	if (!ifs.is_open())
	{
		std::cout << "Error loading " << pPath.getString() << std::endl;
	}

	path.overwrite(pPath);

	String128 mtlPath; mtlPath.overwrite(pPath); mtlPath.shrinkBy(4); mtlPath.append(".mtl",5);

	std::ifstream mtl(mtlPath.getString());

	if (mtl.is_open())
	{
		std::string line;

		while (!mtl.eof())
		{
			std::getline(mtl, line);
			if (line.substr(0, 6) == "newmtl")
			{
				std::string materialName;
				materialName = line.substr(7);

				while (true)
				{
					std::getline(mtl, line);
					std::istringstream iss(line);
					if (iss.str().length() < 2)
						break;
					if (iss.str()[0] == '\n')
						break;
					std::getline(iss, line, ' ');
					if (line == "map_Kd")
					{
						std::getline(iss, line);
						std::string texName;
						int i = line.length() - 4;
						int j = 0;
						while (true)
						{
							if (line[i] == '\\')
								break;
							--i; ++j;
						}
						texName = line.substr(i + 1, j - 1);
						materials.insert(std::make_pair(String64(materialName.c_str()), String32(texName.c_str())));
					}
				}
			}
		}
	}

	Group* currentGroup = nullptr;

	vData.data[0] = new std::vector<glm::fvec3>;
	vData.data[1] = new std::vector<glm::fvec3>;
	vData.data[2] = new std::vector<glm::fvec2>;
	vData.format.size = 8;

	while (!ifs.eof())
	{
		std::string type;
		std::getline(ifs, type, ' ');
		if (type == "o")
		{
			std::string groupName;
			std::getline(ifs, groupName, '_');
			groups.push_back(Group(String64(groupName.c_str())));
			currentGroup = &groups.back();
			std::getline(ifs, groupName);
		}
		else if (type == "usemtl")
		{
			std::string materialName;
			std::getline(ifs, materialName);
			currentGroup->materialName.setToChars(materialName.c_str());
		}
		else if (type == "v")
		{
			std::string xstr, ystr, zstr;
			std::getline(ifs, xstr, ' ');
			std::getline(ifs, ystr, ' ');
			std::getline(ifs, zstr, '\n');

			glm::fvec3 add(std::stof(xstr), std::stof(ystr), std::stof(zstr));

			auto arr = (std::vector<glm::fvec3>*)(vData.data[0]);

			arr->push_back(add);

			//((std::vector<glm::fvec3>*)(vData.data))->push_back(glm::fvec3();
		}
		else if (type == "vt")
		{
			std::string ustr, tstr;
			std::getline(ifs, ustr, ' ');
			std::getline(ifs, tstr, '\n');

			glm::fvec2 add(std::stof(ustr), std::stof(tstr));

			auto arr = (std::vector<glm::fvec2>*)(vData.data[2]);

			arr->push_back(add);

			//data.texCoords.push_back(std::stof(ustr));
			//data.texCoords.push_back(std::stof(tstr));
			//((std::vector<glm::fvec2>*)(vData.data + (2 * sizeof(int*))))->push_back(glm::fvec2(std::stof(ustr), std::stof(tstr)));
		}
		else if (type == "vn")
		{
			std::string xstr, ystr, zstr;
			std::getline(ifs, xstr, ' ');
			std::getline(ifs, ystr, ' ');
			std::getline(ifs, zstr, '\n');

			glm::fvec3 add(std::stof(xstr), std::stof(ystr), std::stof(zstr));

			auto arr = (std::vector<glm::fvec3>*)(vData.data[1]);

			arr->push_back(add);

			//data.normals.push_back(std::stof(xstr));
			//data.normals.push_back(std::stof(ystr));
			//data.normals.push_back(std::stof(zstr));
			//((std::vector<glm::fvec3>*)(vData.data + (1 * sizeof(int*))))->push_back(glm::fvec3(std::stof(xstr), std::stof(ystr), std::stof(ystr)));
		}
		else if (type == "f")
		{
			std::string xstr, ystr, zstr;
			std::getline(ifs, xstr, '/');
			std::getline(ifs, ystr, '/');
			std::getline(ifs, zstr, ' ');
			currentGroup->indices.push_back(abs(std::stoi(xstr)));
			currentGroup->indices.push_back(abs(std::stoi(ystr)));
			currentGroup->indices.push_back(abs(std::stoi(zstr)));
			std::getline(ifs, xstr, '/');
			std::getline(ifs, ystr, '/');
			std::getline(ifs, zstr, ' ');
			currentGroup->indices.push_back(abs(std::stoi(xstr)));
			currentGroup->indices.push_back(abs(std::stoi(ystr)));
			currentGroup->indices.push_back(abs(std::stoi(zstr)));
			std::getline(ifs, xstr, '/');
			std::getline(ifs, ystr, '/');
			std::getline(ifs, zstr, '\n');
			currentGroup->indices.push_back(abs(std::stoi(xstr)));
			currentGroup->indices.push_back(abs(std::stoi(ystr)));
			currentGroup->indices.push_back(abs(std::stoi(zstr)));

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
			data.indices.push_back(abs(std::stoi(zstr)));*/
		}
		else
		{
			std::getline(ifs, type, '\n');
		}
	}
}

void MeshUtility::loadOBJ(String<128>& pPath, s32 objIndex)
{
	if (objIndex > objMeshDatas.size() + 1)
		return;

	auto objMeshData = objMeshDatas[objIndex];
	if (objMeshData == nullptr)
	{
		objMeshData = new OBJMeshData();
	}
	else
	{
		delete objMeshData;
		objMeshData = new OBJMeshData();
	}

	objMeshData->load(pPath);
}

void MeshUtility::binFromObj(s32 objIndex, s32 binIndex)
{
	if (objIndex > objMeshDatas.size() - 1)
		return;

	if (binIndex > meshes.size() - 1)
		return;

	auto objMeshData = objMeshDatas[objIndex];
	auto mesh = meshes[binIndex];

	InterleavedVertexData ivd;
	ivd.fromOBJMeshData(objMeshData);

	//mesh->ivd = ivd;
	//mesh->intData.interlacedData = ivd.data;
	//mesh->intData.size = ivd.size;
	mesh->size = 0;
	//mesh->diskPath.overwrite(String<128>("PATH"));
	//mesh->name.overwrite(String<32>("NAME"));

	for (auto itr = ivd.materialGroups.begin(); itr != ivd.materialGroups.end(); ++itr)
	{
		u32 totalSizeInFloats = 0;
		u32 totalNumVerts = 0;
		for (auto itr2 = itr->second.begin(); itr2 != itr->second.end(); ++itr2)
		{
			totalSizeInFloats += itr2->size;
			totalNumVerts += itr2->size / itr2->format.size;
		}
		Mesh::TriangleList list;
		list.material.matID = PNUU_T_S_N;
		list.data = new float[totalSizeInFloats];
		list.first = 0;
		list.numVerts = totalNumVerts;
		s32 prevSize = 0;
		for (auto itr2 = itr->second.begin(); itr2 != itr->second.end(); ++itr2)
		{
			memcpy(list.data + prevSize, itr2->data, itr2->getDataSizeInBytes());
			prevSize = itr2->getDataSizeInBytes();
		}
		
		list.material.nullTextures(Engine::assets.get2DTex("null"));
		if (itr->second.front().textureName.getLength() != 0)
		{
			list.material.albedo[0] = Engine::assets.get2DTex(itr->second.front().textureName);
			String32 normName; normName.overwrite(itr->second.front().textureName);
			normName.append('N');
			list.material.normal[0] = Engine::assets.get2DTex(normName);
		}

		mesh->triangleLists.push_back(list);
	}



	mesh->name.overwrite(objMeshData->name);
	String128 diskPath("res/model/");
	diskPath.append(mesh->getName());
	diskPath.append(".bin");
	mesh->diskPath.overwrite(diskPath);
}
