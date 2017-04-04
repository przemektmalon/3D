#include "Font.h"

GlyphContainer* Font::requestGlyphs(u16 pCharSize, Text * pUser)
{
	auto find = useTrack.find(pCharSize);
	if (find == useTrack.end())
	{
		auto ret = loadGlyphs(pCharSize);
		useTrack.insert(std::make_pair(pCharSize, std::set<Text*>({ pUser })));
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