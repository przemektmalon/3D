#pragma once
#include "SDL.h"
#include "Types.h"
#include <iostream>
#include <string>
#include <sstream>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\matrix_transform_2d.hpp"

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

		bool fullscreen = false;
		sizeX = 1280; sizeY = 720;
		int posx = 960 - (sizeX / 2), posy = 540 - (sizeY / 2);
		if (fullscreen)
		{
			sizeX = 1920; sizeY = 1080;
			posx = 0; posy = 0;
		}

		sdlWindow = SDL_CreateWindow("Engine!", posx, posy, sizeX, sizeY, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | (SDL_WINDOW_FULLSCREEN && fullscreen));
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

	void screenshot(std::string filename = std::string("screenshot"))
	{
		u32 imageSize = getSizeX() * getSizeY() * 3;
		u32 rowSize = getSizeX() * 3;
		u8* screenshot = new u8[imageSize];
		u8* flipped = new u8[imageSize];

		std::string filepath = "res/screenshot/" + filename + ".bmp";
		glReadPixels(0, 0, getSizeX(), getSizeY(), GL_BGR, GL_UNSIGNED_BYTE, screenshot);

		for (int y = 0; y < getSizeY(); ++y)
		{
			for (int x = 0; x < getSizeX(); ++x)
			{
				u32 incOffset = imageSize - rowSize - (y * rowSize) + (x * 3);
				flipped[y * rowSize + (x * 3)] = screenshot[incOffset];
				flipped[y * rowSize + (x * 3) + 1] = screenshot[incOffset + 1];
				flipped[y * rowSize + (x * 3) + 2] = screenshot[incOffset + 2];
			}
		}

		SDL_Surface * surface = SDL_CreateRGBSurfaceFrom(flipped, getSizeX(), getSizeY(), 8 * 3, getSizeX() * 3, 0, 0, 0, 0);
		SDL_SaveBMP(surface, filepath.c_str());
		SDL_FreeSurface(surface);
		delete[] screenshot;
		delete[] flipped;
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