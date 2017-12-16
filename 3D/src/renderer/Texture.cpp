#include "Texture.hpp"

void Texture2D::makeGLAsset()
{
	glData = new GLTexture2D();
	glData->assignTexture(this);
}
