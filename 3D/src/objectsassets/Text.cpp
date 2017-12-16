#include "Text.hpp"
#include "shaders\TextShader.hpp"
#include "Renderer.hpp"

void Text2D::setStyle(TextStyle& pStyle)
{
	font = pStyle.font;
	charSize = pStyle.charSize;
	colour = pStyle.colour;
	update();
}

void Text2D::setStyleAndOrigin(TextStyle& pStyle)
{
	font = pStyle.font;
	charSize = pStyle.charSize;
	colour = pStyle.colour;
	hasCustomOrigin = pStyle.hasCustomOrigin;
	if (hasCustomOrigin)
	{
		textOrigin = pStyle.textOrigin;
	}
	else
	{
		textOriginPreDef = pStyle.textOriginPreDef;
	}
	update();
}

void Text2D::init()
{
	if (initt)
		return;

	initt = true;

	//string = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n1234567890!@#$%^&*()_\n-=+[]{};':\"\\|<>,./~`";
	string = "\0";
	charSize = 12;
	colour = glm::fvec3(1.f, 1.f, 1.f);
	setTextOrigin(TopLeft);

	//shader = new TextShader();
	shader = &Engine::r->textShader;
	shader->use();

	shader->setModel(glm::fmat4(1.f));
	shader->setView(glm::fmat4(1.f));

	glGenVertexArrays(1, &vao);
	glGenVertexArrays(1, &vaoBBox);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vboBBox);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	auto posAttrib = glGetAttribLocation(shader->getGLID(), "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(posAttrib);

	auto texAttrib = glGetAttribLocation(shader->getGLID(), "texCoord");
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(texAttrib);

	glBindVertexArray(vaoBBox);
	glBindBuffer(GL_ARRAY_BUFFER, vboBBox);

	posAttrib = glGetAttribLocation(shader->getGLID(), "position");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(posAttrib);

	texAttrib = glGetAttribLocation(shader->getGLID(), "texCoord");
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(texAttrib);

	shader->stop();
}

void Text2D::draw()
{
	glBindVertexArray(vao);

	shader->use();
	shader->setColour(colour);
	font->requestGlyphs(charSize, this)->bind();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDrawArrays(GL_QUADS, 0, vboSize);

	if (Engine::cfg.render.drawTextBounds)
	{
		glBindVertexArray(vaoBBox);

		glm::fvec4 c(1.f, 0.f, 1.f, 1.f);
		auto s = (Shape2DShader*)Engine::r->shaderStore.getShader(String32("Shape2DShader"));
		s->use();
		s->setColour(c);
		s->sendUniforms();

		glDrawArrays(GL_LINE_LOOP, 0, 4);
	}

	glBindVertexArray(0);
}

void Text2D::setTextOrigin(Text2D::Origin pOrigin)
{
	updateBoundingBox();
	textOriginPreDef = pOrigin;
	switch (pOrigin)
	{
	case BotLeft:
	{
		setTextOrigin(0, boundingBox.height);
		break;
	}
	case TopLeft:
	{
		setTextOrigin(0, 0);
		break;
	}
	case TopRight:
	{
		setTextOrigin(boundingBox.width, 0);
		break;
	}
	case BotRight:
	{
		setTextOrigin(boundingBox.width, boundingBox.height);
		break;
	}
	case TopMiddle:
	{
		setTextOrigin(boundingBox.width / 2.f, 0);
		break;
	}
	case BotMiddle:
	{
		setTextOrigin(boundingBox.width / 2.f, boundingBox.height);
		break;
	}
	case MiddleLeft:
	{
		setTextOrigin(0, boundingBox.height / 2.f);
		break;
	}
	case MiddleRight:
	{
		setTextOrigin(boundingBox.width, boundingBox.height / 2.f);
		break;
	}
	case MiddleMiddle:
	{
		setTextOrigin(boundingBox.width / 2.f, boundingBox.height / 2.f);
		break;
	}
	}
	hasCustomOrigin = false;
}

void Text2D::updateBoundingBox()
{
	boundingBox.zero();
	if (charSize == 0) { return; }
	if (string.length() == 0) { return; }
	if (font == nullptr) { return; }

	glyphs = font->requestGlyphs(charSize, this);
	auto glyphsTex = glyphs->getTexture();

	auto p = string.c_str();

	auto tpos = position - glm::fvec2(textOrigin);

	glm::ivec2 rgp(tpos);

	boundingBox.zero();
	boundingBox.left = rgp.x;
	boundingBox.top = rgp.y;

	float ascender = glyphs->getAscender();

	glm::ivec2 glyphPos = glyphs->getPosition(*p);
	glm::ivec2 glyphSize = glyphs->getSize(*p);

	glm::fvec2 botLeft(rgp.x + glyphPos.x, rgp.y + ascender + (glyphSize.y - glyphPos.y));
	glm::fvec2 topRight(rgp.x + glyphPos.x + glyphSize.x, rgp.y + ascender - glyphPos.y);

	s32 minX, maxX, minY, maxY;
	minX = botLeft.x; maxX = topRight.x;
	minY = topRight.y; maxY = botLeft.y;

	while (*p != 0)
	{
		if (*p == '\n')
		{
			rgp.x = tpos.x;
			rgp.y += charSize;
		}
		else
		{
			glm::ivec2 glyphPos = glyphs->getPosition(*p);
			glm::ivec2 glyphSize = glyphs->getSize(*p);

			glm::fvec2 botLeft(rgp.x + glyphPos.x, rgp.y + ascender + (glyphSize.y - glyphPos.y));
			glm::fvec2 botRight(rgp.x + glyphPos.x + glyphSize.x, rgp.y + ascender + (glyphSize.y - glyphPos.y));
			glm::fvec2 topRight(rgp.x + glyphPos.x + glyphSize.x, rgp.y + ascender - glyphPos.y);
			glm::fvec2 topLeft(rgp.x + glyphPos.x, rgp.y + ascender - glyphPos.y);

			if (botLeft.x < minX)
				minX = botLeft.x;
			if (botLeft.y > maxY)
				maxY = botLeft.y;

			if (topRight.x > maxX)
				maxX = topRight.x;
			if (topRight.y < minY)
				minY = topRight.y;

			rgp.x += glyphs->getAdvance(*p).x;
			rgp.y += glyphs->getAdvance(*p).y;
		}
		++p;
	}

	boundingBox.left = minX;
	boundingBox.top = minY;
	boundingBox.width = maxX - minX;
	boundingBox.height = maxY - minY;

	const auto bb = boundingBox;

	float bbDataTmp[] = {

		bb.left,			bb.top,				0.f, 0.f, 0.f,
		bb.left + bb.width, bb.top,				0.f, 0.f, 0.f,
		bb.left + bb.width, bb.top + bb.height,	0.f, 0.f, 0.f,
		bb.left,			bb.top + bb.height,	0.f, 0.f, 0.f

	};

	glNamedBufferData(vboBBox, sizeof(bbDataTmp), bbDataTmp, GL_STATIC_DRAW);
}

void Text2D::updateVBO()
{
	boundingBox.zero(); vboSize = 0;
	if (charSize == 0) { return; }
	if (string.length() == 0) { return; }
	if (font == nullptr) { return; }

	int vboNumQuad = string.length();
	int vboNumVert = vboNumQuad * 4;
	int sizeOfVert = 5;
	vboSize = vboNumVert;

	int vboSizeFloats = vboNumVert * sizeOfVert;
	int vboSizeChar = vboNumVert * sizeOfVert * sizeof(float);
	float* vertData = new float[vboSizeFloats];

	glyphs = font->requestGlyphs(charSize, this); // glyphContainers[0];
	auto glyphsTex = glyphs->getTexture();

	int index = 0;
	auto p = string.c_str();

	auto tpos = position - glm::fvec2(textOrigin);

	glm::ivec2 rgp(tpos);

	/*switch (windowOrigin)
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
	}*/

	boundingBox.zero();
	boundingBox.left = rgp.x;
	boundingBox.top = rgp.y;

	float ascender = glyphs->getAscender();

	glm::ivec2 glyphPos = glyphs->getPosition(*p);
	glm::ivec2 glyphSize = glyphs->getSize(*p);

	glm::fvec2 botLeft(rgp.x + glyphPos.x, rgp.y + ascender + (glyphSize.y - glyphPos.y));
	glm::fvec2 topRight(rgp.x + glyphPos.x + glyphSize.x, rgp.y + ascender - glyphPos.y);

	s32 minX, maxX, minY, maxY;
	minX = botLeft.x; maxX = topRight.x;
	minY = topRight.y; maxY = botLeft.y;

	while (*p != 0)
	{
		if (*p == '\n')
		{
			rgp.x = tpos.x;
			rgp.y += charSize;
		}
		else
		{
			glm::ivec2 glyphPos = glyphs->getPosition(*p);
			glm::ivec2 glyphSize = glyphs->getSize(*p);
			glm::fvec2 glyphCoords = glyphs->getCoords(*p);

			auto cOffset = glm::fvec2(glyphSize) / glm::fvec2(glyphsTex->getWidth(), glyphsTex->getHeight());

			glm::fvec2 botLeft(rgp.x + glyphPos.x, rgp.y + ascender + (glyphSize.y - glyphPos.y));
			glm::fvec2 botRight(rgp.x + glyphPos.x + glyphSize.x, rgp.y + ascender + (glyphSize.y - glyphPos.y));
			glm::fvec2 topRight(rgp.x + glyphPos.x + glyphSize.x, rgp.y + ascender - glyphPos.y);
			glm::fvec2 topLeft(rgp.x + glyphPos.x, rgp.y + ascender - glyphPos.y);

			GLfloat vertices[] = {
				//Position						Texcoords
				botLeft.x,  botLeft.y,  0,		glyphCoords.x,				glyphCoords.y + cOffset.y,	// Bot-left
				botRight.x, botRight.y, 0,		glyphCoords.x + cOffset.x,	glyphCoords.y + cOffset.y,	// Bot-right
				topRight.x, topRight.y, 0,		glyphCoords.x + cOffset.x,	glyphCoords.y,				// Top-right
				topLeft.x,  topLeft.y,  0,		glyphCoords.x,				glyphCoords.y				// Top-left
			};
			memcpy((char*)(vertData)+(sizeof(vertices) * index), vertices, sizeof(vertices));

			if (botLeft.x < minX)
				minX = botLeft.x;
			if (botLeft.y > maxY)
				maxY = botLeft.y;

			if (topRight.x > maxX)
				maxX = topRight.x;
			if (topRight.y < minY)
				minY = topRight.y;

			rgp.x += glyphs->getAdvance(*p).x;
			rgp.y += glyphs->getAdvance(*p).y;
		}
		++p;
		++index;
	}

	boundingBox.left = minX;
	boundingBox.top = minY;
	boundingBox.width = maxX - minX;
	boundingBox.height = maxY - minY;

	//updateOrigin();

	const auto bb = boundingBox;

	float bbDataTmp[] = {

		bb.left,			bb.top,				0.f, 0.f, 0.f,
		bb.left + bb.width, bb.top,				0.f, 0.f, 0.f,
		bb.left + bb.width, bb.top + bb.height,	0.f, 0.f, 0.f,
		bb.left,			bb.top + bb.height,	0.f, 0.f, 0.f

	};

	glNamedBufferData(vboBBox, sizeof(bbDataTmp), bbDataTmp, GL_STATIC_DRAW);
	glNamedBufferData(vbo, vboSizeChar, vertData, GL_STATIC_DRAW);

	delete[] vertData;
}
