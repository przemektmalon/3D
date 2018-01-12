#include "Billboard.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "Shape3DShader.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"

Shape3DShader* Billboard::shader;

void Billboard::initGLVAO()
{
	shader = &(Engine::r->shape3DShader);
}

void Billboard::initGL()
{
	shader->use();

	vao.create();
	vao.bind();

	vbo.create();
	vbo.bind();

	vbo.bufferData(sizeof(squareVerts), (void*)squareVerts, GL_STATIC_DRAW);

	vao.addAttrib("p", 3, GL_FLOAT);
	vao.addAttrib("t", 2, GL_FLOAT);

	vao.enableFor(*shader);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Billboard::draw()
{
	auto drawSize = size;

	if (constantSize)
		drawSize *= glm::length(Engine::cam.pos - pos) * 0.04f;

	shader->use();

	glm::fmat4 pvm(1.f);
	pvm = Engine::r->activeCam->projView;

	auto view = Engine::r->activeCam->view;

	glm::fvec3 camUp(view[0][0], view[1][0], view[2][0]);
	glm::fvec3 camRight(view[0][1], view[1][1], view[2][1]);

	vao.bind();
	vbo.bind();

	float* data = new float[20];

	glm::fvec3 p[4]; //Points

	p[0] = pos + (camRight * -0.5f * drawSize.x) + (camUp * -0.5f * drawSize.y);
	p[1] = pos + (camRight * 0.5f * drawSize.x) + (camUp * -0.5f * drawSize.y);
	p[2] = pos + (camRight * 0.5f * drawSize.x) + (camUp * 0.5f * drawSize.y);
	p[3] = pos + (camRight * -0.5f * drawSize.x) + (camUp * 0.5f * drawSize.y);

	memcpy(data, squareVerts, sizeof(squareVerts));

	memcpy(data, &p[0], sizeof(float) * 3);
	memcpy(&data[5], &p[1], sizeof(float) * 3);
	memcpy(&data[10], &p[2], sizeof(float) * 3);
	memcpy(&data[15], &p[3], sizeof(float) * 3);

	vbo.bufferData(sizeof(squareVerts), data, GL_STATIC_DRAW);

	tex->bind(6);

	shader->setPVM(pvm);
	shader->setTex(6);
	shader->sendUniforms();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);

	delete[] data;
}
