#pragma once
#include "Include.h"
#include "Font.h"
#include "ConfigParser.h"

/*class FontStore
{
public:
FontStore() : initialised(false) {}
~FontStore() {}

void initialise()
{
if (initialised)
return;
initialised = true;

ConfigParser fontsCFG;
fontsCFG.parse("fontList.ini");
std::string fontLoc = fontsCFG.getValue(0, "0");
for (int i = 1; i < fontsCFG.getSize(); ++i)
{
auto nf = new Font();
auto fontName = fontsCFG.getValue(0, std::to_string(i));
nf->load(fontLoc + fontName);
if (nf->isValid())
fonts.insert(std::make_pair(std::string(fontName.begin(), fontName.end() - 4), nf));
else
std::cout << "Invalid font location in \"fontList.ini\": " << fontLoc << fontName << std::endl;
}
}

Font* getFont(std::string pName)
{

}

private:

bool initialised;
std::map<std::string, Font*> fonts;
};*/