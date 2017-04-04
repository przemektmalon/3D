#pragma once
#include "Include.h"
#include "Font.h"
#include "Transform.h"
#include "TextShader.h"
#include "Engine.h"
#include "Window.h"
#include "Renderer.h"
#include "Rect.h"

class Text
{
public:
	Text() {}
	~Text() {}


};

class Text2D : public Text
{
public:
	enum Origin { BotLeft, TopLeft, TopRight, BotRight };

	Text2D()
	{

	}
	~Text2D()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}
	void setFont(Font* pFont) { font = pFont; updateVBO(); }
	void setCharSize(u8 pSize) {
		charSize = pSize;
		//glyphContainer = font->requestGlyphs(charSize, this); updateVBO(); 
	}
	void setString(std::string pStr) { string = pStr; updateVBO(); }
	void setPosition(glm::fvec2 pPos) { position = pPos; updateVBO(); }
	glm::fvec2 getPosition() { return position; }
	void setColour(glm::fvec3 pCol) { colour = pCol; }
	glm::fvec3 getColour() { return colour; }
	std::string getString() { return string; }

	void init()
	{
		//string = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n1234567890!@#$%^&*()_\n-=+[]{};':\"\\|<>,./~`";
		string = "";
		charSize = 0;
		//textOrigin = glm::ivec2(0, -14);

		shader = new TextShader();
		shader->use();

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		auto posAttrib = glGetAttribLocation(shader->getGLID(), "position");
		glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(posAttrib);

		auto texAttrib = glGetAttribLocation(shader->getGLID(), "texCoord");
		glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(texAttrib);

		//transform.setTranslation(glm::fvec3(-300, 100, 0));
		//transform.setTranslation(glm::fvec3(0, 500, 0));

		shader->stop();
	}

	void draw()
	{
		glBindVertexArray(vao);

		shader->use();
		shader->setColour(colour);
		font->requestGlyphs(charSize, this)->bind();
		glDrawArrays(GL_QUADS, 0, vboSize);

		glBindVertexArray(0);
	}

	void setWindowOrigin(Origin pO)
	{
		windowOrigin = pO;
		updateVBO();
	}

	void setWindowSize(glm::ivec2 pSize)
	{
		windowSize = pSize;
		updateVBO();
	}

	void setTextOrigin(int pOx, int pOy)
	{
		setTextOrigin(glm::ivec2(pOx, pOy));
	}

	void setTextOrigin(glm::ivec2 pO)
	{
		textOrigin = pO;
		textOrigin.y = -textOrigin.y;
		updateVBO();
	}

	TextShader* shader;

	frect getBoundingBox()
	{
		return boundingBox;
	}

private:

	void updateVBO()
	{
		if (charSize == 0) { return; }

		int vboNumQuad = string.size();
		int vboNumVert = vboNumQuad * 4;
		int sizeOfVert = 5;
		vboSize = vboNumVert;

		int vboSizeFloats = vboNumVert * sizeOfVert;
		int vboSizeChar = vboNumVert * sizeOfVert * sizeof(float);

		float* vertData = new float[vboSizeFloats];

		auto glyphs = font->requestGlyphs(charSize, this); // glyphContainers[0];
		auto glyphsTex = glyphs->getTexture();

		//auto pos = position;

		//glm::fvec3 dirOfText = -glm::cross(up, dirToReader);

		int index = 0;
		auto p = string.c_str();

		auto tpos = position - glm::fvec2(textOrigin);

		glm::ivec2 rgp(tpos);

		switch (windowOrigin)
		{
		case(BotLeft):

			break;
		case(TopLeft):
			rgp.y = windowSize.y - rgp.y;
			tpos.y = rgp.y;
			break;
		case(TopRight):
			rgp.x = windowSize.x - rgp.x;
			rgp.y = windowSize.y - rgp.y;
			tpos.y = rgp.y;
			tpos.x = rgp.x;
			break;
		case(BotRight):
			rgp.x = windowSize.x - rgp.x;
			tpos.x = rgp.x;
			break;
		}

		boundingBox.zero();
		boundingBox.left = rgp.x;
		boundingBox.bot = rgp.y;

		s32 minX, maxX, minY, maxY;
		minX = rgp.x; maxX = rgp.x;
		minY = rgp.y; maxY = rgp.y;

		while (*p != 0)
		{
			if (*p == '\n')
			{
				rgp.x = tpos.x;
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
					glyphPos.x, glyphPos.y, -10,								glyphCoords.x, glyphCoords.y, // Bot-left
					glyphPos.x + glyphSize.x, glyphPos.y, -10,					glyphCoords.x + cOffset.x, glyphCoords.y, // Bot-right
					glyphPos.x + glyphSize.x, glyphPos.y - glyphSize.y, -10,	glyphCoords.x + cOffset.x, glyphCoords.y + cOffset.y,  // Top-right
					glyphPos.x, glyphPos.y - glyphSize.y, -10,					glyphCoords.x, glyphCoords.y + cOffset.y // Top-left
				};
				memcpy((char*)(vertData)+(sizeof(vertices) * index), vertices, sizeof(vertices));

				//boundingBox = boundingBox + frect(glyphPos.x, glyphPos.y, glyphSize.x, glyphSize.y);

				if (glyphPos.x < minX)
					minX = glyphPos.x;
				if (glyphPos.y - glyphSize.y < minY)
					minY = glyphPos.y - glyphSize.y;

				if (glyphPos.y > maxY)
					maxY = glyphPos.y;
				if (glyphPos.x + glyphSize.x > maxX)
					maxX = glyphPos.x + glyphSize.x;

				rgp.x += glyphs->getAdvance(*p).x;
				rgp.y += glyphs->getAdvance(*p).y;
			}
			++p;
			++index;
		}

		boundingBox.left = minX;
		boundingBox.bot = minY;
		boundingBox.width = maxX - minX;
		boundingBox.height = maxY - minY;

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vboSizeChar, vertData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	Origin windowOrigin;
	glm::ivec2 windowSize;

	glm::ivec2 textOrigin;
	glm::fvec2 position;
	glm::fvec2 origin;
	float rotation;
	frect boundingBox;
	glm::fvec3 colour;

	glm::fvec2 relGlyphPos;
	GLuint vbo;
	int vboSize;
	GLuint vao;
	std::string string;
	u16 charSize;
	Font* font;
	GlyphContainer* glyphContainer;
};