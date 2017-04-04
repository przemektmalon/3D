#include "Engine.h"
#include "SDL.h"
#include "Include.h"
#include "Shader.h"
#include "Mesh.h"
#include "Time.h"
#include "QPC.h"
#include "SOIL.h"

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
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
	proj = glm::translate(proj, glm::fvec3(0, 0, 0));

	glm::fmat4 view, projView;

	Mesh m;
	m.loadBinary("MONKEY.bin");
	//m.load("MONKEY.obj");
	//m.saveBinary("MONKEY.bin");

	//for (int i = 0; i < m.intData.size; i+=8)
	//{
	//	printf("%f,%f,%f\n", m.intData.interlacedData[i], m.intData.interlacedData[i + 1], m.intData.interlacedData[i + 2]);
	//	//std::cout << m.intData.interlacedData[i] << "," << m.intData.interlacedData[i + 1] << "," << m.intData.interlacedData[i + 2] << std::endl;
	//}

	std::string fileName("marble.png");
	int width, height;
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	unsigned char* image = SOIL_load_image(fileName.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);

	fileName = std::string("marble.png");
	GLuint specularMap;
	glGenTextures(1, &specularMap);
	glBindTexture(GL_TEXTURE_2D, specularMap);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	image = SOIL_load_image(fileName.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);

	fileName = std::string("marble.png");
	GLuint marble;
	glGenTextures(1, &marble);
	glBindTexture(GL_TEXTURE_2D, marble);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	image = SOIL_load_image(fileName.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);

	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, m.intData.size * sizeof(GLfloat), m.intData.interlacedData, GL_STATIC_DRAW);

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

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GLuint sampler;
	glGenSamplers(1, &sampler);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindSampler(0, sampler);
	glBindSampler(1, sampler);
	glBindSampler(2, sampler);

	auto aa = glGetUniformLocation(s(), "viewPos");
	auto ab = glGetUniformLocation(s(), "material.diffuse");
	auto ac = glGetUniformLocation(s(), "material.specular");
	auto ad = glGetUniformLocation(s(), "material.emit");
	auto ae = glGetUniformLocation(s(), "material.emitStrength");
	auto af = glGetUniformLocation(s(), "material.shininess");
	auto ag = glGetUniformLocation(s(), "proj");

	glUniform1i(ab, 0);
	glUniform1i(ac, 1);
	glUniform1i(ad, 2);
	glUniform1f(ae, 0.3f);
	glUniform1f(af, 32.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularMap);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, marble);

	float pitch = 0, yaw = 0, roll = 0;
	float targetPitch = 0, targetYaw = 0;
	float camYaw = 0;

	Time dt;
	QPC qpc;

	SDL_GL_SetSwapInterval(0);

	s64 tot = 0;
	glm::fvec3 pos;
	glm::ivec2 lastM;
	SDL_Event ev;
	bool quit = false;
	while (!quit)
	{
		qpc.start();
		while (SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_KEYDOWN)
			{
				if (ev.key.keysym.sym == SDLK_ESCAPE)
				{
					quit = true;
				}
				if (ev.key.keysym.sym == SDLK_KP_1)
				{
					glUniform1f(af, 1.f);
				}
				if (ev.key.keysym.sym == SDLK_KP_2)
				{
					glUniform1f(af, 2.0f);
				}
				if (ev.key.keysym.sym == SDLK_KP_3)
				{
					glUniform1f(af, 4.0f);
				}
				if (ev.key.keysym.sym == SDLK_KP_4)
				{
					glUniform1f(af, 8.0f);
				}
				if (ev.key.keysym.sym == SDLK_KP_5)
				{
					glUniform1f(af, 16.0f);
				}
				if (ev.key.keysym.sym == SDLK_KP_6)
				{
					glUniform1f(af, 32.0f);
				}
				if (ev.key.keysym.sym == SDLK_KP_7)
				{
					glUniform1f(af, 64.0f);
				}
				if (ev.key.keysym.sym == SDLK_KP_8)
				{
					glUniform1f(af, 128.0f);
				}
				if (ev.key.keysym.sym == SDLK_KP_9)
				{
					glUniform1f(af, 256.0f);
				}
			}
			if (ev.type == SDL_MOUSEMOTION)
			{
				int x, y;
				auto m = SDL_GetMouseState(&x, &y);

				glm::vec2 mouse_delta = glm::vec2(x, y) - glm::vec2(lastM.x, lastM.y);

				const float mouseX_Sensitivity = 0.002f;
				const float mouseY_Sensitivity = 0.002f;

				targetYaw += mouseX_Sensitivity * mouse_delta.x;
				targetPitch += mouseY_Sensitivity * mouse_delta.y;
			}
		}

		SDL_GetMouseState(&lastM.x, &lastM.y);

		if (lastM.x <= float(window.sizeX) * 0.1f || lastM.x >= float(window.sizeX) * 0.9f)
		{
			SDL_WarpMouseInWindow(window.sdlWindow, window.sizeX / 2, lastM.y);
			lastM.x = window.sizeX / 2;
		}

		if (lastM.y <= float(window.sizeY) * 0.1f || lastM.y >= float(window.sizeY) * 0.9f)
		{
			SDL_WarpMouseInWindow(window.sdlWindow, lastM.x, window.sizeY / 2);
			lastM.y = window.sizeY / 2;
		}

		//SDL_WarpMouseInWindow(window.sdlWindow, window.sizeX / 2, window.sizeY / 2);

		SDL_ShowCursor(0);

		glm::fmat4 matRoll = glm::rotate(glm::fmat4(), roll, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::fmat4 matPitch = glm::rotate(glm::fmat4(), pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::fmat4 matYaw = glm::rotate(glm::fmat4(), yaw, glm::vec3(0.0f, 1.0f, 0.0f));

		float moveSpeed = 100;

		auto keyboardState = SDL_GetKeyboardState(NULL);

		auto move = glm::fvec3(glm::fvec4(0, 0, moveSpeed * dt.getSeconds(), 1) * matYaw);
		pos -= move * float(keyboardState[SDL_SCANCODE_W]);

		move = glm::cross(glm::fvec3(glm::fvec4(0, 0, moveSpeed * dt.getSeconds(), 1) * matYaw), glm::fvec3(0, 1, 0));
		pos += move * float(keyboardState[SDL_SCANCODE_A]);

		move = glm::fvec3(glm::fvec4(0, 0, moveSpeed * dt.getSeconds(), 1) * matYaw);
		pos += move * float(keyboardState[SDL_SCANCODE_S]);

		move = glm::cross(glm::fvec3(glm::fvec4(0, 0, moveSpeed * dt.getSeconds(), 1) * matYaw), glm::fvec3(0, 1, 0));
		pos -= move * float(keyboardState[SDL_SCANCODE_D]);

		pos.y += moveSpeed * dt.getSeconds() * float(keyboardState[SDL_SCANCODE_R]);

		pos.y -= moveSpeed * dt.getSeconds() * float(keyboardState[SDL_SCANCODE_F]);

		glm::fmat4 translate = glm::fmat4(1.0f);
		translate = glm::translate(translate, -pos);

		targetPitch = glm::clamp(targetPitch, -float(PI) / 2.f, float(PI) / 2.f);

		pitch = glm::mix(pitch, targetPitch, 10.f * dt.getSeconds());
		yaw = glm::mix(yaw, targetYaw, 10.f * dt.getSeconds());
		//scal = lerp(10.f * et, scal, scales);

		glm::fmat4 rotate = matRoll * matPitch * matYaw;
		view = rotate * translate;

		rotate = matYaw;

		projView = proj * view;

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glClearColor(0.05, 0.05, 0.05, 1.0);

		glUniform3f(aa, pos.x, pos.y, pos.z);

		model = glm::rotate(model, dt.getSecondsf(), glm::fvec3(0, 1, 0));

		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projView));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glDrawArrays(GL_TRIANGLES, 0, m.data.numVert);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//m.draw();

		window.swapBuffers();

		dt.setMicroSeconds(qpc.getElapsedTime());

		tot += dt.getMicroSeconds();
		if (tot > 100000)
		{
			std::cout << 1.f / dt.getSecondsf() << std::endl;
			tot = 0;
		}
	}
};