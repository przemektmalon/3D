#include "Font.hpp"

GlyphContainer* Font::requestGlyphs(u16 pCharSize, Text * pUser)
{
		auto find = useTrack.find(pCharSize);
	if (find == useTrack.end())
	{
		auto ret = loadGlyphs(pCharSize);
		useTrack.insert(std::make_pair(pCharSize, std::set<Text*>({ pUser })));
		return ret;
	}
	else
	{
		find->second.insert(pUser);
		return &glyphContainers.at(pCharSize);
	}
}

void Font::releaseGlyphs(u16 pCharSize, Text* pUser)
{
	auto find = useTrack.find(pCharSize);
	if (find == useTrack.end())
	{

	}
	else
	{
		find->second.erase(pUser);
		if (find->second.size() == 0)
		{
			glyphContainers.erase(pCharSize);
		}
	}
}

void GlyphContainer::load(u16 pCharSize, FT_Face pFace)
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
	tempTex.createFromStream(GL_R8, charSize * 10, charSize * 10, GL_RED, GL_UNSIGNED_BYTE, NULL, 0, 1);
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

	height = pFace->size->metrics.height >> 6;
	maxYSize += lineYSize;
	maxXSize = lineXSize > maxXSize ? lineXSize : maxXSize;
	glyphs.makeGLAsset();
	glyphs.glData->createFromStream(GL_R8, maxXSize, maxYSize, GL_RED, GL_UNSIGNED_BYTE, NULL, 0, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	//tempTex.saveToFile("A", true);
	glCopyImageSubData(tempTex.getGLID(), GL_TEXTURE_2D, 0, 0, 0, 0, glyphs.glData->getGLID(), GL_TEXTURE_2D, 0, 0, 0, 0, maxXSize, maxYSize, 1);
	//glyphs.saveToFile("B", true);
	tempTex.release();
}

