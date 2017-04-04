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

	void bind() { 
		glBindTextureUnit(12, glyphs.getGLID()); 
	}

	void load(u16 pCharSize, FT_Face pFace);

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