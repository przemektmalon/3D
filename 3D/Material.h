#pragma once
#include "Engine.h"
#include "SOIL.h"
#include "glm\glm.hpp"

class Texture
{
public:
	Texture() { glGenTextures(1, &id); }
	Texture(std::string name) { glGenTextures(1, &id); loadTexture(name); }
	~Texture() { glDeleteTextures(1, &id); }

	void loadTexture(std::string fileName)
	{
		glBindTexture(GL_TEXTURE_2D, id);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		unsigned char* image = SOIL_load_image(fileName.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}

	void loadFromMem(float* source, s32 width, s32 height)
	{
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, source);
	}

	inline glm::vec2 getSize() const { return glm::vec2(width, height); }

	s32 width, height;
	GLuint id;
};

class Material
{
public:
	Material() {}
	~Material() {}

	Texture diffuseTex;
	Texture specularTex;
	Texture emissionTex;
	float shininess;
};