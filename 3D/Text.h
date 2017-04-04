#pragma once
#include "Include.h"
#include "Font.h"
#include "Transform.h"
#include "TextShader.h"

class Text
{
public:
	Text()
	{

	}
	~Text()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}
	void setFont(Font* pFont) { font = pFont; updateVBO(); }
	void setCharSize(u8 pSize) {
		//charSize = pSize; glyphContainer = font->requestGlyphs(charSize, this); updateVBO(); 
	}
	void setString(std::string pStr) { string = pStr; updateVBO(); }

	std::string getString() { return string; }

	void init()
	{
		//string = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n1234567890!@#$%^&*()_\n-=+[]{};':\"\\|<>,./~`";
		string = "~~~~~~~~";
		charSize = 16;

		shader = new TextShader();
		shader->use();

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		auto posAttrib = glGetAttribLocation(shader->operator()(), "position");
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(posAttrib);

		auto texAttrib = glGetAttribLocation(shader->operator()(), "texCoord");
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(texAttrib);

		transform.setTranslation(glm::fvec3(-300, 100, 0));

		shader->stop();
	}

	void draw()
	{
		glBindVertexArray(vao);

		shader->use();
		//font->getGlyphs(charSize)->bind();
		glDrawArrays(GL_QUADS, 0, vboSize);

		glBindVertexArray(0);
	}

	TextShader* shader;

private:

	void updateVBO()
	{
		int vboNumQuad = string.size();
		int vboNumVert = vboNumQuad * 4;
		int sizeOfVert = 5;
		vboSize = vboNumVert;

		int vboSizeFloats = vboNumVert * sizeOfVert;
		int vboSizeChar = vboNumVert * sizeOfVert * sizeof(float);

		float* vertData = new float[vboSizeFloats];

		auto glyphs = glyphContainer;
		auto glyphsTex = glyphs->getTexture();

		auto pos = transform.getTranslation();

		//glm::fvec3 dirOfText = -glm::cross(up, dirToReader);

		int index = 0;
		auto p = string.c_str();
		glm::ivec2 rgp(0, 200);
		while (*p != 0)
		{
			if (*p == '\n')
			{
				rgp.x = 0;
				rgp.y -= charSize;
			}
			else
			{
				auto glyphPos = rgp + glyphs->getPosition(*p);
				auto glyphSize = glyphs->getSize(*p);
				auto glyphCoords = glyphs->getCoords(*p);

				auto cOffset = glm::fvec2(glyphSize) / glm::fvec2(glyphsTex->getWidth(), glyphsTex->getHeight());

				GLfloat vertices[] = {
					//Position													Texcoords
					glyphPos.x, glyphPos.y, -100,								glyphCoords.x, glyphCoords.y, // Bot-left
					glyphPos.x + glyphSize.x, glyphPos.y, -100,					glyphCoords.x + cOffset.x, glyphCoords.y, // Bot-right
					glyphPos.x + glyphSize.x, glyphPos.y - glyphSize.y, -100,	glyphCoords.x + cOffset.x, glyphCoords.y + cOffset.y,  // Top-right
					glyphPos.x, glyphPos.y - glyphSize.y, -100,					glyphCoords.x, glyphCoords.y + cOffset.y // Top-left
				};

				memcpy((char*)(vertData)+(sizeof(vertices) * index), vertices, sizeof(vertices));

				rgp.x += glyphs->getAdvance(*p).x;
				rgp.y += glyphs->getAdvance(*p).y;
			}
			++p;
			++index;
		}

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vboSizeChar, vertData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	GLuint vbo;
	int vboSize;
	GLuint vao;
	Transform transform;
	std::string string;
	u16 charSize;
	Font* font;
	GlyphContainer* glyphContainer;
};