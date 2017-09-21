#pragma once
#include "Text.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "Framebuffer.hpp"

class Text3D : public Text2D
{
public:

	void initGL()
	{
		fb.setResolution(glm::ivec2(1, 1));
		fb.attachTexture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0);
		fb.checkStatus();
	}

	void reinitGL()
	{
		fb.destroyAllAttachments();
		fb.attachTexture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0);
	}

	void setPosition(glm::fvec3 pPosition)
	{
		position3 = pPosition;
	}

	glm::fvec3 getPosition()
	{
		return position3;
	}

	void drawToFB()
	{
		fb.bind();
		fb.clear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(vao);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		updateVBO();

		shader->use();
		shader->setColour(colour);

		//glm::fmat4 modelView = Engine::r->activeCam->view * glm::translate(glm::scale(glm::fmat4(),glm::fvec3(0.02,0.02,1.f)), position3);
		glm::fmat4 modelView = Engine::r->activeCam->view * glm::translate(glm::fmat4(), position3) * glm::scale(glm::fmat4(), glm::fvec3(0.02, 0.02, 0.2f));

		glm::fmat4 proj = glm::ortho<float>(0, boundingBox.width, 0, boundingBox.height, 0, 10);

		shader->setView(glm::fmat4());
		shader->setProj(proj);

		font->requestGlyphs(charSize, this)->bind();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDrawArrays(GL_QUADS, 0, vboSize);

		/*if (Engine::r->config.drawTextBounds)
		{
			glBindVertexArray(vaoBBox);

			glm::fvec4 c(1.f, 0.f, 1.f, 1.f);
			auto s = (Shape2DShader*)Engine::r->shaderStore.getShader(String32("Shape2DShader"));
			s->use();
			s->setColour(c);
			s->sendUniforms();

			glDrawArrays(GL_LINE_LOOP, 0, 4);
		}*/

		glBindVertexArray(0);

		Engine::r->fboDefault.bind();
	}

	void draw()
	{
		glm::fmat4 model = glm::scale(glm::fmat4(), glm::fvec3(0.02, 0.02, 0.2f));

		Engine::r->shape3DShader.use();
		Engine::r->shape3DShader.setPVM(Engine::r->activeCam->projView * model);
		Engine::r->shape3DShader.sendUniforms();

		glm::fmat4 pvm(1.f);
		pvm = Engine::r->activeCam->projView;

		auto view = Engine::r->activeCam->view;

		glm::fvec3 camUp(view[0][0], view[1][0], view[2][0]);
		glm::fvec3 camRight(view[0][1], view[1][1], view[2][1]);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		float* data = new float[20];

		glm::fvec3 p[4]; //Points

		auto pos = position3;
		auto size = glm::fvec2(boundingBox.width, boundingBox.height);

		p[0] = pos + (camRight * -0.5f * size.x) + (camUp * -0.5f * size.y);
		p[1] = pos + (camRight * 0.5f * size.x) + (camUp * -0.5f * size.y);
		p[2] = pos + (camRight * 0.5f * size.x) + (camUp * 0.5f * size.y);
		p[3] = pos + (camRight * -0.5f * size.x) + (camUp * 0.5f * size.y);

		memcpy(data, squareVerts, sizeof(squareVerts));

		memcpy(data, &p[0], sizeof(float) * 3);
		memcpy(&data[5], &p[1], sizeof(float) * 3);
		memcpy(&data[10], &p[2], sizeof(float) * 3);
		memcpy(&data[15], &p[3], sizeof(float) * 3);

		glBufferData(GL_ARRAY_BUFFER, sizeof(squareVerts), data, GL_STATIC_DRAW);

		//fb.renderbufferAttachments[0].bind();

		auto shader = &Engine::r->shape3DShader;

		shader->setPVM(pvm);
		shader->sendUniforms();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDrawArrays(GL_QUADS, 0, 4);

		delete[] data;
	}

	void updateVBO()
	{
		//boundingBox.zero();
		vboSize = 0;
		if (charSize == 0) { return; }
		if (string.getLength() == 0) { return; }
		if (font == nullptr) { return; }

		int vboNumQuad = string.getLength();
		int vboNumVert = vboNumQuad * 4;
		int sizeOfVert = 5;
		vboSize = vboNumVert;

		int vboSizeFloats = vboNumVert * sizeOfVert;
		int vboSizeChar = vboNumVert * sizeOfVert * sizeof(float);
		float* vertData = new float[vboSizeFloats];

		auto glyphs = font->requestGlyphs(charSize, this); // glyphContainers[0];
		auto glyphsTex = glyphs->getTexture();

		int index = 0;
		auto c = string.getString();

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

		//boundingBox.zero();
		//boundingBox.left = rgp.x;
		//boundingBox.top = rgp.y;

		s32 minX, maxX, minY, maxY;
		minX = rgp.x; maxX = rgp.x;
		minY = rgp.y; maxY = rgp.y;

		auto view = Engine::r->activeCam->view;

		glm::fvec3 camUp(view[0][0], view[1][0], view[2][0]);
		glm::fvec3 camRight(view[0][1], view[1][1], view[2][1]);

		while (*c != 0)
		{
			if (*c == '\n')
			{
				rgp.x = tpos.x;
				rgp.y -= charSize;
			}
			else
			{
				glm::uvec2 glyphPos = rgp + glyphs->getPosition(*c);
				glm::uvec2 glyphSize = glyphs->getSize(*c);
				glm::fvec2 glyphCoords = glyphs->getCoords(*c);

				auto cOffset = glm::fvec2(glyphSize) / glm::fvec2(glyphsTex->getWidth(), glyphsTex->getHeight());

				GLfloat vertices[] = {
					//Position													Texcoords
					glyphPos.x, glyphPos.y, 0,									glyphCoords.x, glyphCoords.y, // Bot-left
					glyphPos.x + glyphSize.x, glyphPos.y, 0,					glyphCoords.x + cOffset.x, glyphCoords.y, // Bot-right
					glyphPos.x + glyphSize.x, glyphPos.y - glyphSize.y, 0,		glyphCoords.x + cOffset.x, glyphCoords.y + cOffset.y,  // Top-right
					glyphPos.x, glyphPos.y - glyphSize.y, 0,					glyphCoords.x, glyphCoords.y + cOffset.y // Top-left
				};
				memcpy((char*)(vertData)+(sizeof(vertices) * index), vertices, sizeof(vertices));

				if (glyphPos.x < minX)
					minX = glyphPos.x;
				if (glyphPos.y - glyphSize.y < minY)
					minY = glyphPos.y - glyphSize.y;

				if (glyphPos.y > maxY)
					maxY = glyphPos.y;
				if (glyphPos.x + glyphSize.x > maxX)
					maxX = glyphPos.x + glyphSize.x;

				rgp.x += glyphs->getAdvance(*c).x;
				rgp.y += glyphs->getAdvance(*c).y;
			}
			++c;
			++index;
		}

		auto prevBB = boundingBox;

		boundingBox.left = minX;
		boundingBox.top = minY;
		boundingBox.width = maxX - minX;
		boundingBox.height = maxY - minY;

		if (fb.resolution != glm::ivec2(boundingBox.size + glm::fvec2(1)))
		{
			fb.setResolution(boundingBox.size + glm::fvec2(1));
			reinitGL();
		}

		//updateOrigin();

		const auto bb = boundingBox;

		float bbDataTmp[] = { bb.left, bb.top, 0.f, 0.f,0.f,
			bb.left + bb.width, bb.top, 0.f,0.f,0.f,
			bb.left + bb.width, bb.top + bb.height,0.f,0.f,0.f,
			bb.left,bb.top + bb.height,0.f,0.f,0.f };

		glNamedBufferData(vboBBox, sizeof(bbDataTmp), bbDataTmp, GL_STATIC_DRAW);



		glNamedBufferData(vbo, vboSizeChar, vertData, GL_STATIC_DRAW);



		delete[] vertData;
	}


private:
	glm::fvec3 position3;

	Framebuffer fb;
	
};