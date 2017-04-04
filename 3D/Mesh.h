#pragma once
#include "Include.h"
#include <string>
#include <fstream>
#include "Shader.h"

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

class Mesh
{
public:
	Mesh() {}
	~Mesh() {}

	void load(std::string objPath)
	{

		std::ifstream ifs(objPath);

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
				data.indices.push_back(std::stoi(xstr));
				data.indices.push_back(std::stoi(ystr));
				data.indices.push_back(std::stoi(zstr));
				std::getline(ifs, xstr, '/');
				std::getline(ifs, ystr, '/');
				std::getline(ifs, zstr, ' ');
				data.indices.push_back(std::stoi(xstr));
				data.indices.push_back(std::stoi(ystr));
				data.indices.push_back(std::stoi(zstr));
				std::getline(ifs, xstr, '/');
				std::getline(ifs, ystr, '/');
				std::getline(ifs, zstr, '\n');
				data.indices.push_back(std::stoi(xstr));
				data.indices.push_back(std::stoi(ystr));
				data.indices.push_back(std::stoi(zstr));
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

			std::cout << data.verts[index] << " " << data.verts[index + 1] << " " << data.verts[index + 2] << std::endl;

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

		//glGenVertexArrays(1, &vao);
		//glBindVertexArray(vao);

		//glGenBuffers(1, &vbo);
		//glBindBuffer(GL_ARRAY_BUFFER, vbo);
		//glBufferData(vbo, intData.size, glvertices, GL_STATIC_DRAW);

		//Shader shaderProgram; shaderProgram.load("min2");

		//auto posAttrib = glGetAttribLocation(shaderProgram(), "p");
		//glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
		//glEnableVertexAttribArray(posAttrib);

		//auto texAttrib = glGetAttribLocation(shaderProgram(), "t");
		//glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		//glEnableVertexAttribArray(texAttrib);

		//auto norAttrib = glGetAttribLocation(shaderProgram(), "n");
		//glVertexAttribPointer(norAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
		//glEnableVertexAttribArray(norAttrib);

		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glBindVertexArray(0);
	}

	void draw()
	{
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glDrawArrays(GL_TRIANGLES, 0, data.numVert);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	//private:

	ObjectData data;
	InterlacedObjectData intData;

	GLuint vbo;
	GLuint vao;
};