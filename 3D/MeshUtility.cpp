
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <io.h>
#include <ios>
#include "MeshUtility.h"
#include "C:/Programming/Libs/GLM/glm/common.hpp"

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

	vData.data[0] = new std::vector<glm::fvec3>;
	vData.data[1] = new std::vector<glm::fvec3>;
	vData.data[2] = new std::vector<glm::fvec2>;

	vData.format.size = 8;

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
			indices.push_back(abs(std::stoi(xstr)));
			indices.push_back(abs(std::stoi(ystr)));
			indices.push_back(abs(std::stoi(zstr)));
			std::getline(ifs, xstr, '/');
			std::getline(ifs, ystr, '/');
			std::getline(ifs, zstr, ' ');
			indices.push_back(abs(std::stoi(xstr)));
			indices.push_back(abs(std::stoi(ystr)));
			indices.push_back(abs(std::stoi(zstr)));
			std::getline(ifs, xstr, '/');
			std::getline(ifs, ystr, '/');
			std::getline(ifs, zstr, '\n');
			indices.push_back(abs(std::stoi(xstr)));
			indices.push_back(abs(std::stoi(ystr)));
			indices.push_back(abs(std::stoi(zstr)));

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

	//data.numVert = data.indices.size() / 3;
	//data.numTris = data.numVert / 3;

	/*int numVert = data.numVert;
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
	}*/

	//auto glVertsSize = numVert * vertSize;

	//std::cout << glVertsSize << std::endl;
	//std::cout << data.numVert << std::endl;
	//std::cout << data.numTris << std::endl;


	/*char* path = pPath.getString();
	int pathLength = 0;

	while (*path != '\0')
	{
		pathLength++;
		path++;
	}
	pathLength++;

	char* binPathName = new char[pathLength];
	path = pPath.getString();
	for (int i = 0; i < pathLength - 5; ++i)
	{
		binPathName[i] = path[i];
		if (binPathName[i] == '\\')
			binPathName[i] = '/';
	}

	binPathName[pathLength - 5] = '.';
	binPathName[pathLength - 4] = 'b';
	binPathName[pathLength - 3] = 'i';
	binPathName[pathLength - 2] = 'n';
	binPathName[pathLength - 1] = '\0';

	for (int i = 0; i < pathLength; ++i)
	{
		std::cout << binPathName[i];
	}*/

	//std::ofstream ofs(binPathName, std::ios_base::binary);
	//ofs.write((char*)&glVertsSize, sizeof(glVertsSize));
	//ofs.write((char*)glvertices, glVertsSize * sizeof(float));
	//ofs.close();
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