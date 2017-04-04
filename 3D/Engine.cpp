#include "Engine.h"
#include "SDL.h"
#include "Include.h"
#include "Shader.h"
#include "Mesh.h"
#include "Time.h"
#include "QPC.h"
#include "SOIL.h"
#include "Camera.h"
#include "Renderer.h"
#include <chrono>
#include "Font.h"
#include "Text.h"
#include "FontStore.h"

#define MODEL_PATH std::string("res/model/")

FT_Library Engine::ftLib;
//FontStore Engine::fontStore;
Camera Engine::defaultOrthoCam;
//Text Engine::t;
Window Engine::window;
Shader Engine::s;
Shader Engine::testShader;
Shader Engine::gPassShader;
bool Engine::wf;
std::mt19937_64 Engine::rand;
s64 Engine::startTime;
Mesh Engine::uSphere;
bool Engine::movingLight;

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

void specifyScreenVertexAttributes(GLuint shaderProgram)
{
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
}

void Engine::mainLoop()
{
	glewExperimental = GL_TRUE;
	glewInit();

	rand.seed(rand.default_seed);

	startTime = std::chrono::system_clock::now().time_since_epoch().count();

	//FT_Init_FreeType(&ftLib);
	//fontStore.initialise();

	//Font font;
	//font.load(size, "gor.ttf", fc);

	//t.init();
	//t.setFont(&font);
	//t.setCharSize(size);
	//t.setString("Hello!");

	s.load("res/shader/Standard", "res/shader/Standard");
	gPassShader.load("res/shader/gBufferPass", "res/shader/gBufferPass");
	testShader.load("res/shader/test", "res/shader/test");
	glUseProgram(s());
	//Mesh m;
	//m.loadBinary("MONKEY.bin");
	////m.load("MONKEY2.obj");
	////m.saveBinary("MONKEY.bin");

	//Mesh m2;
	//m2.load("GROUND.obj");
	//m2.transform.setRoll(PI);
	//m2.transform.setScale(glm::fvec3(10.f));
	//m2.transform.setTranslation(glm::fvec3(0, -80, 0));

	Mesh buddha(MODEL_PATH + "buddha.bin");
	Mesh dragon(MODEL_PATH + "DRG.bin");
	Mesh ballpyr(MODEL_PATH + "ballpyr.bin");
	Mesh table;
	table.load(MODEL_PATH + "table.obj");
	//ballpyr.load(MODEL_PATH + "ballpyr.obj");
	//ballpyr.saveBinary(MODEL_PATH + "ballpyr.bin");
	Mesh floor;
	floor.load(MODEL_PATH + "Plane.obj");
	Mesh surf;
	surf.load(MODEL_PATH + "surface.obj");

	uSphere.load(MODEL_PATH + "UnitSphere.obj");

	SDL_GL_SetSwapInterval(1);

	glViewport(0, 0, window.getSizeX(), window.getSizeY());

	std::string fileName("wd.jpg");
	int width, height;
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	unsigned char* image = SOIL_load_image(fileName.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);

	fileName = std::string("wd.jpg");
	GLuint specularMap;
	glGenTextures(1, &specularMap);
	glBindTexture(GL_TEXTURE_2D, specularMap);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	image = SOIL_load_image(fileName.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);

	fileName = std::string("m.png");
	GLuint marble;
	glGenTextures(1, &marble);
	glBindTexture(GL_TEXTURE_2D, marble);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	image = SOIL_load_image(fileName.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);

	std::vector<glm::fmat4> inst;
	for (int i = 0; i < 1; ++i)
	{
		auto in = glm::translate(glm::fmat4(), glm::fvec3(i * 150, 30, 0));
		in = glm::scale(in, glm::fvec3(4.f));
		in = glm::rotate(in, float(PI) / 5.f, glm::fvec3(0, 1.f, 0));
		inst.push_back(in);
	}
	MasterRenderer r;
	//r.entities.insert(std::make_pair(&buddha, inst));
	std::vector<glm::fmat4> inst2;
	for (int i = 0; i < 1; ++i)
	{
		auto in = glm::translate(glm::fmat4(), glm::fvec3(40, 35, -150));
		in = glm::scale(in, glm::fvec3(10.f));
		in = glm::rotate(in, float(PI) / 4.f, glm::fvec3(0, 1, 0));
		inst2.push_back(in);
	}
	r.entities.insert(std::make_pair(&dragon, inst2));
	std::vector<glm::fmat4> inst3;
	inst3.push_back(glm::scale(glm::translate(glm::fmat4(), glm::fvec3(10, -10, 10)), glm::fvec3(50)));
	r.entities.insert(std::make_pair(&floor, inst3));
	//std::vector<glm::fmat4> inst4;
	//inst4.push_back(glm::scale(glm::translate(glm::fmat4(), glm::fvec3(-30, -5, 70)), glm::fvec3(0.8)));
	//r.entities.insert(std::make_pair(&ballpyr, inst4));
	//std::vector<glm::fmat4> inst5;
	//inst5.push_back(glm::rotate(glm::scale(glm::translate(glm::fmat4(), glm::fvec3(0, -88, 0)), glm::fvec3(50)), glm::radians(-32.f), glm::fvec3(0, 1, 0)));
	//r.entities.insert(std::make_pair(&table, inst5));

	GLuint sampler;
	glGenSamplers(1, &sampler);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glBindSampler(0, sampler);
	glBindSampler(1, sampler);
	glBindSampler(2, sampler);
	glBindSampler(3, sampler);

	GLuint postSampler;
	glGenSamplers(1, &postSampler);
	glSamplerParameteri(postSampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(postSampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(postSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(postSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindSampler(4, postSampler);
	glBindSampler(5, postSampler);

	GLuint cubemapSampler;
	glGenSamplers(1, &cubemapSampler);
	glSamplerParameteri(cubemapSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(cubemapSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(cubemapSampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(cubemapSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(cubemapSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindSampler(6, cubemapSampler);

	GLuint textSampler;
	glCreateSamplers(1, &textSampler);
	glSamplerParameteri(textSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(textSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(textSampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(textSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(textSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindSampler(12, textSampler);

	GLuint shadowSampler;
	glCreateSamplers(1, &shadowSampler);
	glSamplerParameteri(shadowSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glSamplerParameteri(shadowSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glSamplerParameterfv(shadowSampler, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindSampler(8, shadowSampler);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);

	Camera cam;
	cam.initialiseProj(float(window.getSizeX()) / float(window.getSizeY()));
	cam.calculateViewRays();
	r.initialiseRenderer(1, &window, cam);

	defaultOrthoCam.initaliseOrtho(window.getSizeX(), window.getSizeY());

	Time dt;
	QPC qpc;

	s64 tot = 0;

	glm::ivec2 lastM;
	SDL_Event ev;
	bool quit = false;
	wf = false;

	auto expval = glGetUniformLocation(s(), "exposure");
	float exposure = 1.f;

	std::string stringg;

	while (!quit)
	{
		qpc.start();
		while (SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_KEYDOWN)
			{
				if (ev.key.keysym.sym == SDLK_BACKSPACE)
				{
					if (stringg.size() > 0)
					{
						stringg.assign(stringg.begin(), stringg.end() - 1);
						//t.setString(stringg);
					}
				}
				if (ev.key.keysym.sym == SDLK_ESCAPE)
				{
					quit = true;
				}
				if (ev.key.keysym.sym == SDLK_KP_1)
				{
					wf = true;
				}
				if (ev.key.keysym.sym == SDLK_KP_2)
				{
					wf = false;
				}
				if (ev.key.keysym.sym == SDLK_KP_4)
				{
					movingLight = true;
				}
				if (ev.key.keysym.sym == SDLK_KP_5)
				{
					movingLight = false;
				}
				if (ev.key.keysym.sym == SDLK_q)
				{
					glUseProgram(Engine::s());
					auto val = glGetUniformLocation(s(), "filmic");
					glUniform1i(val, 0);
				}
				if (ev.key.keysym.sym == SDLK_e)
				{
					glUseProgram(Engine::s());
					auto val = glGetUniformLocation(s(), "filmic");
					glUniform1i(val, 1);
				}
				if (ev.key.keysym.sym == SDLK_PERIOD)
				{
					window.screenshot();
				}
			}
			if (ev.type == SDL_WINDOWEVENT)
			{
				if (ev.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
				{
					std::cout << "FOCUS GAINED" << std::endl;
					int x, y;
					SDL_GetWindowPosition(window.sdlWindow, &x, &y);
					SDL_WarpMouseGlobal(x + window.sizeX / 2, y + window.sizeY / 2);
					SDL_ShowCursor(0);
				}
				if (ev.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
				{
					std::cout << "FOCUS LOST" << std::endl;
					SDL_ShowCursor(1);
				}
			}
			if (ev.type == SDL_MOUSEMOTION)
			{
				u32 mask = SDL_GetWindowFlags(window.sdlWindow);
				if (((mask & SDL_WINDOW_INPUT_FOCUS) == SDL_WINDOW_INPUT_FOCUS))
				{
					int x, y;
					auto m = SDL_GetMouseState(&x, &y);

					//glm::vec2 mouse_delta = glm::vec2(x, y) - glm::vec2(lastM.x, lastM.y);
					glm::ivec2 mouse_delta = glm::ivec2(x, y) - glm::ivec2(window.sizeX / 2, window.sizeY / 2);

					const float mouseX_Sensitivity = 0.002f;
					const float mouseY_Sensitivity = 0.002f;

					cam.targetYaw += mouseX_Sensitivity * mouse_delta.x;
					cam.targetPitch += mouseY_Sensitivity * mouse_delta.y;

					SDL_WarpMouseInWindow(window.sdlWindow, window.sizeX / 2, window.sizeY / 2);
					SDL_ShowCursor(0);
				}
			}
			if (ev.type == SDL_TEXTINPUT)
			{
				stringg += ev.text.text[0];
				//t.setString(stringg);
			}
		}
		//SDL_GetMouseFocus() == window.sdlWindow && 

		const float moveSpeed = 800;

		auto keyboardState = SDL_GetKeyboardState(NULL);

		auto move = glm::fvec3(glm::fvec4(0, 0, moveSpeed * dt.getSeconds(), 1) * cam.matYaw);
		cam.targetPos -= move * float(keyboardState[SDL_SCANCODE_W]);

		move = glm::cross(glm::fvec3(glm::fvec4(0, 0, moveSpeed * dt.getSeconds(), 1) * cam.matYaw), glm::fvec3(0, 1, 0));
		cam.targetPos += move * float(keyboardState[SDL_SCANCODE_A]);

		move = glm::fvec3(glm::fvec4(0, 0, moveSpeed * dt.getSeconds(), 1) * cam.matYaw);
		cam.targetPos += move * float(keyboardState[SDL_SCANCODE_S]);

		move = glm::cross(glm::fvec3(glm::fvec4(0, 0, moveSpeed * dt.getSeconds(), 1) * cam.matYaw), glm::fvec3(0, 1, 0));
		cam.targetPos -= move * float(keyboardState[SDL_SCANCODE_D]);

		cam.targetPos.y += moveSpeed * dt.getSeconds() * float(keyboardState[SDL_SCANCODE_R]);

		cam.targetPos.y -= moveSpeed * dt.getSeconds() * float(keyboardState[SDL_SCANCODE_F]);

		exposure += float(keyboardState[SDL_SCANCODE_X]) * dt.getSeconds();
		exposure -= float(keyboardState[SDL_SCANCODE_Z]) * dt.getSeconds();
		exposure = max(0.f, exposure);
		s.use();
		glUniform1f(expval, exposure);
		glUniform1f(expval, exposure);
		s.stop();

		r.ssaoShader.setIntensity(r.ssaoShader.intensity + (float(keyboardState[SDL_SCANCODE_B]) * 5 * dt.getSeconds()));
		r.ssaoShader.setIntensity(r.ssaoShader.intensity - (float(keyboardState[SDL_SCANCODE_V]) * 5 * dt.getSeconds()));
		r.ssaoShader.intensity = max(0.f, r.ssaoShader.intensity);

		r.ssaoShader.setRadius(r.ssaoShader.radius + (float(keyboardState[SDL_SCANCODE_H]) * 5 * dt.getSeconds()));
		r.ssaoShader.setRadius(r.ssaoShader.radius - (float(keyboardState[SDL_SCANCODE_G]) * 5 * dt.getSeconds()));
		r.ssaoShader.radius = max(0.f, r.ssaoShader.radius);

		cam.update(dt);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, marble);

		r.render(cam);

		dt.setMicroSeconds(qpc.getElapsedTime());
	}
};