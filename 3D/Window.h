#pragma once
#include "SDL.h"
#include "Types.h"
#include <iostream>
#include <string>
#include <sstream>
#include <GL\glew.h>
#include "Include.h"

class Window
{
public:
	Window() : windowCreated(false) {}
	~Window() {}

	void createWindow()
	{
		if (windowCreated)
			return;

		windowCreated = true;

		if (SDL_Init(SDL_INIT_VIDEO) != 0) {
			std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
			return;
		}

		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
		//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 1);

		bool fullscreen = true;
		sizeX = 800; sizeY = 600;
		int posx = 960 - (sizeX / 2), posy = 540 - (sizeY / 2);
		if (fullscreen)
		{
			sizeX = 1920; sizeY = 1080;
			posx = 0; posy = 0;
		}

		sdlWindow = SDL_CreateWindow("Engine!", posx, posy, sizeX, sizeY, SDL_WINDOW_OPENGL | (SDL_WINDOW_RESIZABLE & !fullscreen) | (SDL_WINDOW_FULLSCREEN && fullscreen));
		if (sdlWindow == nullptr) {
			std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
			SDL_Quit();
			return;
		}

		glContext = SDL_GL_CreateContext(sdlWindow);
		SDL_GL_MakeCurrent(sdlWindow, glContext);

		std::cout << "GL context created" << std::endl;

		GLint maj, min;
		glGetIntegerv(GL_MAJOR_VERSION, &maj);
		glGetIntegerv(GL_MINOR_VERSION, &min);

		std::cout << "GL version " << maj << "." << min << std::endl;

		auto glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);
		std::cout << "GLSL version " << glsl << std::endl;

		std::cout << "Screen Resolution: " << sizeX << "x" << sizeY << std::endl;
	}

	inline void swapBuffers()
	{
		SDL_GL_SwapWindow(sdlWindow);
	}

	inline u32 getSizeX() { return sizeX; }
	inline u32 getSizeY() { return sizeY; }
	inline glm::ivec2 getSize() { return glm::ivec2(sizeX, sizeY); }

	//private:

	bool windowCreated;
	u32 sizeX, sizeY;
	SDL_Window* sdlWindow;
	SDL_GLContext glContext;
};