#include "Font.h"

GlyphContainer* Font::requestGlyphs(u16 pCharSize, Text * pTarget)
{
	return glyphContainers.find(pCharSize)->second;

	/*auto glyphFind = glyphContainers.find(pCharSize);
	auto trackFind = useTrack.find(pCharSize);
	if (trackFind == useTrack.end())
	{
	if (glyphFind == glyphContainers.end())
	{
	std::cout << "GLYPHS REQUESTED: GLYPH CONTAINER DOESN'T EXIST, TEXT OBJECT NOT TRACKED, GLYPHS CREATED, TEXT OBJECT TRACKED, GLYPHS RETURNED" << std::endl;
	auto newGlyphs = loadGlyphs(pCharSize);
	useTrack.insert(std::make_pair(pCharSize, std::list<Text*>()));
	useTrack[pCharSize].push_back(pTarget);
	return newGlyphs;
	}
	else
	assert(false);
	}
	else
	{
	if (glyphFind == glyphContainers.end())
	assert(false);
	else
	{
	auto trackSpecificFind = std::find(trackFind->second.begin(), trackFind->second.end(), pTarget);
	if (trackSpecificFind == trackFind->second.end())
	{
	std::cout << "GLYPHS REQUESTED: TEXT OBJECT NOT TRACKED(FIRST REQUEST), GLYPHS RETURNED" << std::endl;
	trackFind->second.push_back(pTarget);
	return glyphFind->second;
	}
	else
	{
	std::cout << "GLYPHS REQUESTED: TEXT OBJECT ALREADY TRACKED, GLYPHS RETURNED" << std::endl;
	return glyphFind->second;
	}
	}
	}*/
}
