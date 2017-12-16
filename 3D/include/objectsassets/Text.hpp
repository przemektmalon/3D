#pragma once
#include "GLIncludes.hpp"
#include "Font.hpp"
#include "Rect.hpp"

class TextShader;

class Text2D
{
public:
	enum Origin { BotLeft, TopLeft, TopRight, BotRight, TopMiddle, BotMiddle, MiddleLeft, MiddleRight, MiddleMiddle };

	class TextStyle
	{
	public:
		TextStyle() {}
		TextStyle(Font* pFont, u16 pCharSize, glm::fvec3 pColour = glm::fvec3(1.f,1.f,1.f) , bool pHasCustomOrigin = false, Text2D::Origin pTextOriginPreDef = Text2D::Origin::TopLeft, glm::ivec2 pTextOrigin = glm::ivec2(0, 0)) 
		: font(pFont), charSize(pCharSize), colour(pColour), hasCustomOrigin(pHasCustomOrigin), textOriginPreDef(pTextOriginPreDef), textOrigin(pTextOrigin) {}

		Font* font;
		u16 charSize;
		Text2D::Origin textOriginPreDef;
		glm::ivec2 textOrigin;
		bool hasCustomOrigin;
		glm::fvec3 colour;
	};

	Text2D()
	{
		font = nullptr;
		glyphs = nullptr;
		initt = false;
		string.resize(64);
		hasCustomOrigin = true;
	}
	~Text2D()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}
	void setFont(Font* pFont) { font = pFont; update(); }
	void setCharSize(u8 pSize) { charSize = pSize; update(); }
	void setString(std::string pStr) { string = pStr; update(); }
	void setString(StringGeneric& pStr) { string = pStr.getString(); update(); }
	void setPosition(glm::fvec2 pPos) { position = pPos; update(); }
	u8 getCharSize() { return charSize;}
	u16 getHeight() { return boundingBox.height; }
	u16 getMaxHeight() {
		if (!font || (charSize == 0)) { return 0; }
		return font->requestGlyphs(charSize, this)->getHeight();
	}
	glm::fvec2 getPosition() { return position; }
	void setColour(glm::fvec3 pCol) { colour = pCol; }
	glm::fvec3 getColour() { return colour; }
	std::string& getString() { return string; }
	Font* getFont() { return font; }

	void setStyle(TextStyle& pStyle);
	void setStyleAndOrigin(TextStyle& pStyle);

	void init();

	void draw();

	void setWindowOrigin(Origin pO)
	{
		windowOrigin = pO;
		updateVBO();
	}

	void setTextOrigin(int pOx, int pOy)
	{
		setTextOrigin(glm::ivec2(pOx, pOy));
	}

	void setTextOrigin(glm::ivec2 pO)
	{
		hasCustomOrigin = true;
		textOrigin = pO;
		textOrigin.y = textOrigin.y;
		updateVBO();
	}

	void setTextOrigin(Text2D::Origin pOrigin);

	TextShader* shader;

	frect getBoundingBox()
	{
		return boundingBox;
	}

	void forceUpdate()
	{
		updateOrigin();
	}

	GlyphContainer* getGlyphs()
	{
		return glyphs;
	}

private:

	bool updateOrigin()
	{
		if (!hasCustomOrigin)
		{
			setTextOrigin(textOriginPreDef);
			return true;
		}
		return false;
	}

	void update()
	{
		if (!updateOrigin())
		{
			updateBoundingBox();
			updateVBO();
		}
	}

	void updateBoundingBox();

	void updateVBO();

protected:

	Origin windowOrigin;

	Origin textOriginPreDef;
	bool hasCustomOrigin;
	glm::ivec2 textOrigin;
	glm::fvec2 position;
	glm::fvec2 origin;
	float rotation;
	frect boundingBox;
	glm::fvec3 colour;
	bool initt;

	glm::fvec2 relGlyphPos;
	GLuint vboBBox;
	GLuint vbo;
	int vboSize;
	GLuint vao;
	GLuint vaoBBox;
	std::string string;
	u16 charSize;
	Font* font;
	GlyphContainer* glyphs;
};

