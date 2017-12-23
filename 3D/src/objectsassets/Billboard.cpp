#include "Billboard.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "Shape3DShader.hpp"
#include "Texture.hpp"

Shape3DShader* Billboard::shader;

void Billboard::initGLVAO()
{
	shader = &(Engine::r->shape3DShader);
}

void Billboard::initGL()
{
	shader->use();

	glCreateVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glCreateBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVerts), squareVerts, GL_STATIC_DRAW);

	auto posAttrib = glGetAttribLocation(shader->getGLID(), "p");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

	auto texAttrib = glGetAttribLocation(shader->getGLID(), "t");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Billboard::draw()
{
	shader->use();

	glm::fmat4 pvm(1.f);
	pvm = Engine::r->activeCam->projView;

	auto view = Engine::r->activeCam->view;

	glm::fvec3 camUp(view[0][0], view[1][0], view[2][0]);
	glm::fvec3 camRight(view[0][1], view[1][1], view[2][1]);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	float* data = new float[20];

	glm::fvec3 p[4]; //Points

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

	tex->bind(6);

	shader->setPVM(pvm);
	shader->setTex(6);
	shader->sendUniforms();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);

	delete[] data;
}
