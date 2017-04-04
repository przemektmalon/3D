#include "Engine.h"
#include "SDL.h"
#include "Include.h"
#include "Shader.h"
#include "Mesh.h"

Window Engine::window;

int main(int argc, char *argv[])
{
	Engine::start();
	return 0;
}

void Engine::start()
{
	window.createWindow();
	mainLoop();
}

void Engine::mainLoop()
{
	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader s;
	s.load("min2");
	glUseProgram(s());

	//float verts[] = { -100, -100, 0, 0, 0, 0, 0, 0,
	//	100, -100, 0, 0, 0, 0, 0, 0,
	//	0, 100, 0, 0, 0, 0, 0, 0,
	//	-100, -100, 0, 0, 0, 0, 0, 0,
	//	-200, 100, 0, 0, 0, 0, 0, 0,
	//	0, 100, 0, 0, 0, 0, 0, 0 };

	//GLuint vbo;
	//glGenBuffers(1, &vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	//auto posAt = glGetAttribLocation(s(), "position");
	//glVertexAttribPointer(posAt, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	//glEnableVertexAttribArray(posAt);

	auto projLoc = glGetUniformLocation(s(), "proj");
	auto modelLoc = glGetUniformLocation(s(), "model");

	glm::fmat4 proj, model;
	//proj = glm::ortho(-1280.f / 2.f, 1280.f / 2.f, -720.f / 2.f, 720.f / 2.f, 0.1.f, 2000.f);
	proj = glm::perspective(float(PI) / 2.f, float(window.sizeX) / float(window.sizeY), 1.f, 10000.f);
	proj = glm::translate(proj, glm::fvec3(0, 0, -100));

	glm::fmat4 view, projView;

	Mesh m; m.load("CUBE.obj");

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, m.intData.size, m.intData.interlacedData, GL_STATIC_DRAW);

	//glGenVertexArrays(1, &m.vao);
	//glBindVertexArray(m.vao);

	//glGenBuffers(1, &m.vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
	//glBufferData(m.vbo, m.intData.size, m.intData.interlacedData, GL_STATIC_DRAW);

	auto posAttrib = glGetAttribLocation(s(), "p");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(posAttrib);

	auto texAttrib = glGetAttribLocation(s(), "t");
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(texAttrib);

	auto norAttrib = glGetAttribLocation(s(), "n");
	glVertexAttribPointer(norAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(norAttrib);

	SDL_Event ev;
	bool quit = false;
	while (!quit)
	{
		while (SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_KEYDOWN)
			{
				if (ev.key.keysym.sym == SDLK_ESCAPE)
				{
					quit = true;
				}
			}
		}

		auto keyboardState = SDL_GetKeyboardState(NULL);
		if (keyboardState[SDL_SCANCODE_W])
		{
			view = glm::translate(view, glm::fvec3(0, 0, 1));
		}
		if (keyboardState[SDL_SCANCODE_A])
		{
			view = glm::translate(view, glm::fvec3(1, 0, 0));
		}
		if (keyboardState[SDL_SCANCODE_S])
		{
			view = glm::translate(view, glm::fvec3(0, 0, -1));
		}
		if (keyboardState[SDL_SCANCODE_D])
		{
			view = glm::translate(view, glm::fvec3(-1, 0, 0));
		}
		if (keyboardState[SDL_SCANCODE_R])
		{
			view = glm::translate(view, glm::fvec3(0, -1, 0));
		}
		if (keyboardState[SDL_SCANCODE_F])
		{
			view = glm::translate(view, glm::fvec3(0, 1, 0));
		}

		projView = proj * view;

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glClearColor(0.05, 0.05, 0.05, 1.0);

		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projView));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glDrawArrays(GL_TRIANGLES, 0, m.data.numVert);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//m.draw();

		window.swapBuffers();
	}
};