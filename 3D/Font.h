#pragma once
#include "Include.h"
#include <algorithm>
#include <ft2build.h>
#include "Texture.h"
#include "Engine.h"
#include "Asset.h"
#include FT_FREETYPE_H

#define NO_PRINTABLE_CHARS 95

class GlyphContainer
{
public:
	GlyphContainer() { }
	GlyphContainer(u16 pCharSize, FT_Face pFace) { load(pCharSize, pFace); }
	~GlyphContainer() { }

	GLTexture2D* getTexture() { return &glyphs; }

	GLuint getGlyph(unsigned char pAscSymbol) {}

	glm::ivec2 getSize(unsigned char pAscSymbol)
	{
		return sizes[pAscSymbol - 32];
	}

	glm::ivec2 getPosition(unsigned char pAscSymbol)
	{
		return positions[pAscSymbol - 32];
	}

	glm::ivec2 getAdvance(unsigned char pAscSymbol)
	{
		return advances[pAscSymbol - 32];
	}

	glm::fvec2 getCoords(u8 pAscSymbol)
	{
		return glm::fvec2(coords[pAscSymbol - 32]) / glm::fvec2(glyphs.getWidth(), glyphs.getHeight());
	}

	void bind() { glBindTextureUnit(12, glyphs.getGLID()); }

	void load(u16 pCharSize, FT_Face pFace)
	{
		charSize = pCharSize;

		if (pFace == nullptr)
		{
			assert(false);
		}

		FT_Set_Pixel_Sizes(pFace, 0, charSize);

		int maxXSize = 0;
		int maxYSize = 0;
		int lineXSize = 0;
		int lineYSize = 0;

		int xOffset = 0;
		int yOffset = 0;

		GLTexture2D tempTex;
		tempTex.createFromStream(GL_RED, charSize * 10, charSize * 10, GL_RGB, GL_UNSIGNED_BYTE, NULL, 0, 1);
		tempTex.bind();
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		const int storeResolutionX = 12;

		for (char i = 0; i < NO_PRINTABLE_CHARS; ++i)
		{
			int x = i % storeResolutionX;
			int y = floor(i / storeResolutionX);

			if (x == 0)
			{
				xOffset = 0;
				yOffset += lineYSize + 1;
				maxYSize += lineYSize + 1;
				maxXSize = lineXSize > maxXSize ? lineXSize : maxXSize;
				lineXSize = 0;
				lineYSize = 0;
			}

			char ascSymbol = i + (unsigned char)(32);


			FT_Error err = FT_Load_Char(pFace, ascSymbol, FT_LOAD_RENDER);

			lineXSize += pFace->glyph->bitmap.width + 1;
			lineYSize = pFace->glyph->bitmap.rows > lineYSize ? pFace->glyph->bitmap.rows : lineYSize;

			sizes[i].x = pFace->glyph->bitmap.width;
			sizes[i].y = pFace->glyph->bitmap.rows;

			positions[i].x = pFace->glyph->bitmap_left;
			positions[i].y = pFace->glyph->bitmap_top;

			advances[i].x = pFace->glyph->advance.x >> 6;
			advances[i].y = pFace->glyph->advance.y >> 6;

			coords[i].x = xOffset;
			coords[i].y = yOffset;

			glTexSubImage2D(GL_TEXTURE_2D, 0, coords[i].x, coords[i].y, sizes[i].x, sizes[i].y, GL_RED, GL_UNSIGNED_BYTE, pFace->glyph->bitmap.buffer);

			xOffset += sizes[i].x + 1;
		}

		maxYSize += lineYSize;
		maxXSize = lineXSize > maxXSize ? lineXSize : maxXSize;
		glyphs.createFromStream(GL_RED, maxXSize, maxYSize, GL_RGB, GL_UNSIGNED_BYTE, NULL, 0, 1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		tempTex.saveToFile("A", true);
		glCopyImageSubData(tempTex.getGLID(), GL_TEXTURE_2D, 0, 0, 0, 0, glyphs.getGLID(), GL_TEXTURE_2D, 0, 0, 0, 0, maxXSize, maxYSize, 1);
		glyphs.saveToFile("B", true);
		tempTex.release();
	}

	void release() {}

private:

	u16 charSize;
	GLTexture2D glyphs;
	glm::ivec2 sizes[NO_PRINTABLE_CHARS];
	glm::ivec2 positions[NO_PRINTABLE_CHARS];
	glm::ivec2 advances[NO_PRINTABLE_CHARS];
	glm::ivec2 coords[NO_PRINTABLE_CHARS];

};

class Text;

class Font : public Asset
{
	friend class AssetManager;
public:
	Font(String128& pPath, String32& pName) : ftFace(nullptr)
	{
		prepare(pPath, pName);
	}
	~Font() {}

	GlyphContainer* requestGlyphs(u16 pCharSize, Text* pUser);
	void releaseGlyphs(u16 pCharSize, Text* pUser);

protected:

	void load()
	{
		if (ftFace == nullptr)
		{
			auto err = FT_New_Face(Engine::ftLib, diskPath.getString(), 0, &ftFace);
			if (err)
				assert(false);
		}
	}

private:

	GlyphContainer* loadGlyphs(u16 pCharSize)
	{
		auto ins = glyphContainers.insert(std::make_pair(pCharSize, GlyphContainer(pCharSize, ftFace)));
		return &ins.first->second;
	}

	FT_Face ftFace;
	std::map<u16, std::set<Text*>> useTrack;
	std::map<u16, GlyphContainer> glyphContainers;
	GlyphContainer gl;
};