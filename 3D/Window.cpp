#pragma once
//#include "SDL.h"
#include "Types.h"
#include <iostream>
#include <string>
#include <sstream>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include "glm\gtc\type_ptr.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\matrix_transform_2d.hpp"
#include <time.h>
#include <Windows.h>
#include "resource.h"
#include <SDKDDKVer.h>
#include "SOIL.h"

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <strsafe.h>

#include "Keyboard.h"
#include "Mouse.h"

#define MM_CREATE_GL_CONTEXT 3001


/*extern "C" {

extern int sfwgl_ext_EXT_swap_control;
extern int sfwgl_ext_EXT_framebuffer_sRGB;
extern int sfwgl_ext_ARB_framebuffer_sRGB;
extern int sfwgl_ext_ARB_multisample;
extern int sfwgl_ext_ARB_pixel_format;
extern int sfwgl_ext_ARB_pbuffer;
extern int sfwgl_ext_ARB_create_context;
extern int sfwgl_ext_ARB_create_context_profile;



#define WGL_FRAMEBUFFER_SRGB_CAPABLE_EXT 0x20A9

#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB 0x20A9

#define WGL_SAMPLES_ARB 0x2042
#define WGL_SAMPLE_BUFFERS_ARB 0x2041

#define WGL_ACCELERATION_ARB 0x2003
#define WGL_ACCUM_ALPHA_BITS_ARB 0x2021
#define WGL_ACCUM_BITS_ARB 0x201D
#define WGL_ACCUM_BLUE_BITS_ARB 0x2020
#define WGL_ACCUM_GREEN_BITS_ARB 0x201F
#define WGL_ACCUM_RED_BITS_ARB 0x201E
#define WGL_ALPHA_BITS_ARB 0x201B
#define WGL_ALPHA_SHIFT_ARB 0x201C
#define WGL_AUX_BUFFERS_ARB 0x2024
#define WGL_BLUE_BITS_ARB 0x2019
#define WGL_BLUE_SHIFT_ARB 0x201A
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_DRAW_TO_BITMAP_ARB 0x2002
#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_FULL_ACCELERATION_ARB 0x2027
#define WGL_GENERIC_ACCELERATION_ARB 0x2026
#define WGL_GREEN_BITS_ARB 0x2017
#define WGL_GREEN_SHIFT_ARB 0x2018
#define WGL_NEED_PALETTE_ARB 0x2004
#define WGL_NEED_SYSTEM_PALETTE_ARB 0x2005
#define WGL_NO_ACCELERATION_ARB 0x2025
#define WGL_NUMBER_OVERLAYS_ARB 0x2008
#define WGL_NUMBER_PIXEL_FORMATS_ARB 0x2000
#define WGL_NUMBER_UNDERLAYS_ARB 0x2009
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_RED_BITS_ARB 0x2015
#define WGL_RED_SHIFT_ARB 0x2016
#define WGL_SHARE_ACCUM_ARB 0x200E
#define WGL_SHARE_DEPTH_ARB 0x200C
#define WGL_SHARE_STENCIL_ARB 0x200D
#define WGL_STENCIL_BITS_ARB 0x2023
#define WGL_STEREO_ARB 0x2012
#define WGL_SUPPORT_GDI_ARB 0x200F
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_SWAP_COPY_ARB 0x2029
#define WGL_SWAP_EXCHANGE_ARB 0x2028
#define WGL_SWAP_LAYER_BUFFERS_ARB 0x2006
#define WGL_SWAP_METHOD_ARB 0x2007
#define WGL_SWAP_UNDEFINED_ARB 0x202A
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB 0x203A
#define WGL_TRANSPARENT_ARB 0x200A
#define WGL_TRANSPARENT_BLUE_VALUE_ARB 0x2039
#define WGL_TRANSPARENT_GREEN_VALUE_ARB 0x2038
#define WGL_TRANSPARENT_INDEX_VALUE_ARB 0x203B
#define WGL_TRANSPARENT_RED_VALUE_ARB 0x2037
#define WGL_TYPE_COLORINDEX_ARB 0x202C
#define WGL_TYPE_RGBA_ARB 0x202B

#define WGL_DRAW_TO_PBUFFER_ARB 0x202D
#define WGL_MAX_PBUFFER_HEIGHT_ARB 0x2030
#define WGL_MAX_PBUFFER_PIXELS_ARB 0x202E
#define WGL_MAX_PBUFFER_WIDTH_ARB 0x202F
#define WGL_PBUFFER_HEIGHT_ARB 0x2035
#define WGL_PBUFFER_LARGEST_ARB 0x2033
#define WGL_PBUFFER_LOST_ARB 0x2036
#define WGL_PBUFFER_WIDTH_ARB 0x2034

#define WGL_CONTEXT_DEBUG_BIT_ARB 0x00000001
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define WGL_CONTEXT_LAYER_PLANE_ARB 0x2093
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_ERROR_INVALID_VERSION_ARB 0x2095

#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_ERROR_INVALID_PROFILE_ARB 0x2096

#ifndef WGL_EXT_swap_control
#define WGL_EXT_swap_control 1
extern int (WINAPI *sf_ptrc_wglGetSwapIntervalEXT)(void);
#define wglGetSwapIntervalEXT sf_ptrc_wglGetSwapIntervalEXT
extern BOOL(WINAPI *sf_ptrc_wglSwapIntervalEXT)(int);
#define wglSwapIntervalEXT sf_ptrc_wglSwapIntervalEXT
#endif // WGL_EXT_swap_control


#ifndef WGL_ARB_pixel_format
#define WGL_ARB_pixel_format 1
extern BOOL(WINAPI *sf_ptrc_wglChoosePixelFormatARB)(HDC, const int*, const FLOAT*, UINT, int*, UINT*);
#define wglChoosePixelFormatARB sf_ptrc_wglChoosePixelFormatARB
extern BOOL(WINAPI *sf_ptrc_wglGetPixelFormatAttribfvARB)(HDC, int, int, UINT, const int*, FLOAT*);
#define wglGetPixelFormatAttribfvARB sf_ptrc_wglGetPixelFormatAttribfvARB
extern BOOL(WINAPI *sf_ptrc_wglGetPixelFormatAttribivARB)(HDC, int, int, UINT, const int*, int*);
#define wglGetPixelFormatAttribivARB sf_ptrc_wglGetPixelFormatAttribivARB
#endif // WGL_ARB_pixel_format

#ifndef WGL_ARB_create_context
#define WGL_ARB_create_context 1
extern HGLRC(WINAPI *sf_ptrc_wglCreateContextAttribsARB)(HDC, HGLRC, const int*);
#define wglCreateContextAttribsARB sf_ptrc_wglCreateContextAttribsARB
#endif // WGL_ARB_create_context

}*/

class Window
{
public:
	Window() : windowCreated(false) {}
	~Window() {}

	static Window* engineWindow;

	BOOL processMessages()
	{
		MSG msg;
		if (PeekMessage(&msg, windowHandle, 0, 0, PM_REMOVE)) {
			//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			//{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			//}
			return true;
		}
		return false;
	}

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void setUpNewWindow()
	{

	}

	void deleteWindow()
	{
		if (windowCreated)
		{
			wglDeleteContext(glContextHandle);
			DeleteDC(deviceContextHandle);
			windowCreated = false;
		}
	}

	void createWindow(HINSTANCE pHInstance, LPCTSTR pWindowName, int pXPos, int pYPos, int pWidth, int pHeight, DWORD pWindowStyle = 0)
	{
		if (windowCreated)
			return;

		windowCreated = true;

		engineWindow = const_cast<Window*>(this);
		windowClass.lpfnWndProc = WndProc;
		windowClass.hInstance = pHInstance;
		windowClass.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
		windowClass.lpszClassName = LPCSTR("WINDOW_CLASS_NAME");
		windowClass.style = CS_OWNDC;
		if (!RegisterClass(&windowClass))
			int x = 10;

		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);
		auto horizontal = desktop.right;
		auto vertical = desktop.bottom;

		bool fullscreen = false;

		desiredClientArea.x = pWidth;
		desiredClientArea.y = pHeight;

		posX = (horizontal / 2) - (desiredClientArea.x / 2);
		posY = (vertical / 2) - (desiredClientArea.y / 2);
		if (fullscreen)
		{
			desiredClientArea.x = horizontal; desiredClientArea.y = vertical;
			posX = 0; posY = 0;
		}

		windowHandle = CreateWindowEx(0, LPCSTR(windowClass.lpszClassName), "WINDOW_WINDOW_NAME", pWindowStyle, pXPos, pYPos, pWidth, pHeight, 0, 0, pHInstance, 0);


		RECT ca = getClientArea();
		actualClientArea.x = ca.right;
		actualClientArea.y = ca.bottom;

		forceClientAreaToDesired();
		createGLContext();

		ShowWindow(windowHandle, 1);



		std::cout << "Screen Resolution: " << desiredClientArea.x << "x" << desiredClientArea.y << std::endl;

		//SWAP INTERVAL VSYNC
	}

	void createGLContext()
	{
		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DEPTH_DONTCARE,    //Flags
			PFD_TYPE_RGBA,             //The kind of framebuffer. RGBA or palette.
			32,                        //Colordepth of the framebuffer.
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			0,                        //Number of bits for the depthbuffer
			0,                        //Number of bits for the stencilbuffer
			0,                        //Number of Aux buffers in the framebuffer.
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};

		deviceContextHandle = GetDC(windowHandle);

		int  letWindowsChooseThisPixelFormat;
		letWindowsChooseThisPixelFormat = ChoosePixelFormat(deviceContextHandle, &pfd);
		SetPixelFormat(deviceContextHandle, letWindowsChooseThisPixelFormat, &pfd);

		HGLRC ourOpenGLRenderingContext = wglCreateContext(deviceContextHandle);
		wglMakeCurrent(deviceContextHandle, ourOpenGLRenderingContext);

		std::cout << "GL context created" << std::endl;

		GLint maj, min;
		glGetIntegerv(GL_MAJOR_VERSION, &maj);
		glGetIntegerv(GL_MINOR_VERSION, &min);

		std::cout << "GL version " << maj << "." << min << std::endl;

		auto glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);
		std::cout << "GLSL version " << glsl << std::endl;
	}

	inline void swapBuffers()
	{
		wglSwapLayerBuffers(deviceContextHandle, WGL_SWAP_MAIN_PLANE);
	}

	bool hasFocus()
	{
		return windowHasFocus;
	}

	void escapePressed()
	{
		PostMessage(windowHandle, WM_CLOSE, 0, 0);
	}

	void setResolution(glm::ivec2 pRes)
	{
		desiredClientArea = pRes;
		forceClientAreaToDesired();
	}

	RECT getClientArea()
	{
		RECT wr2;
		auto er2 = GetClientRect(windowHandle, &wr2);
		return wr2;
	}

	void forceClientAreaToDesired()
	{
		RECT wr;
		auto er = GetWindowRect(windowHandle, &wr);
		RECT wr2 = getClientArea();
		POINT mouse;

		if (wr2.right != desiredClientArea.x || wr2.bottom != desiredClientArea.y)
		{
			glm::ivec2 windowSpaceToAdd = desiredClientArea - glm::ivec2(wr2.right, wr2.bottom);
			//windowSpaceToAdd = glm::ivec2(0);
			RECT wr3;
			wr3.left = wr.left; wr3.top = wr.top;
			wr3.right = (wr.right - wr.left) + windowSpaceToAdd.x; wr3.bottom = (wr.bottom - wr.top) + windowSpaceToAdd.y;
			//AdjustWindowRectEx(&wr, WS_OVERLAPPEDWINDOW, false, 0);
			MoveWindow(windowHandle, wr3.left, wr3.top, wr3.right, wr3.bottom, 0);
		}
		actualClientArea.x = desiredClientArea.x;
		actualClientArea.y = desiredClientArea.y;
	}

	bool isMouseInClientArea()
	{
		RECT wr;
		auto er = GetWindowRect(windowHandle, &wr);
		RECT wr2;
		auto er2 = GetClientRect(windowHandle, &wr2);
		POINT mouse;

		/*if (wr2.right != engineWindow->desiredClientArea.x || wr2.bottom != engineWindow->desiredClientArea.y)
		{
		glm::ivec2 windowSpaceToAdd = engineWindow->desiredClientArea - glm::ivec2(wr2.right, wr2.bottom);
		RECT wr3;
		wr3.left = wr.left; wr3.top = wr.top; wr3.right = wr.left + engineWindow->desiredClientArea.x + windowSpaceToAdd.x; wr3.bottom = wr.top + engineWindow->desiredClientArea.y + windowSpaceToAdd.y;
		auto errr = AdjustWindowRectEx(&wr, WS_OVERLAPPEDWINDOW, false, 0);
		int x = 10;
		MoveWindow(engineWindow->windowHandle, wr3.left, wr3.top, engineWindow->desiredClientArea.x + windowSpaceToAdd.x, engineWindow->desiredClientArea.y + windowSpaceToAdd.y, 0);
		}*/
		GetCursorPos(&mouse);

		RECT clientRect;
		clientRect.left = wr.right - wr2.right;
		clientRect.top = wr.bottom - wr2.bottom;
		clientRect.right = clientRect.left + wr2.right;
		clientRect.bottom = clientRect.top + wr2.bottom;

		if (mouse.y > clientRect.top && mouse.y < clientRect.bottom && mouse.x > clientRect.left && mouse.x < clientRect.right)
		{
			return true;
		}
		return false;
	}

	void captureMouseFocus()
	{
		windowHasFocus = true;
		//SetCursorPos(getPosX() + (getSizeX() / 2), getPosY() + (getSizeY() / 2));
		//ShowCursor(false);
		//OutputDebugString("HIDE(CAPTURE) CURSOR\n");
	}

	void registerInputDevices()
	{
		RAWINPUTDEVICE Rid[1];

		Rid[0].usUsagePage = 0x01;
		Rid[0].usUsage = 0x02;
		Rid[0].dwFlags = 0;
		Rid[0].hwndTarget = 0;

		if (!RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]))) {
			//registration failed. Call GetLastError for the cause of the error
			int x = 10;
		}
	}

	void screenshot()
	{
		time_t now = time(0);
		tm  tstruct;
		char buf[80];
		localtime_s(&tstruct, &now);
		OutputDebugString("SCREENSHOT");
		strftime(buf, sizeof(buf), "%F_%H;%M;%S", &tstruct);
		auto fileName = std::string("Screenshot_") + std::string(buf);
		screenshot(fileName);
	}

	void screenshot(std::string fileName)
	{
		u32 imageSize = getSizeX() * getSizeY() * 3;
		u32 rowSize = getSizeX() * 3;
		u8* screenshot = new u8[imageSize];
		//u8* flipped = new u8[imageSize];

		std::string filepath = "res/screenshot/" + fileName + ".bmp";
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glReadBuffer(GL_BACK);
		glReadPixels(0, 0, getSizeX(), getSizeY(), GL_BGR, GL_UNSIGNED_BYTE, screenshot);

		/*for (int y = 0; y < getSizeY(); ++y)
		{
		for (int x = 0; x < getSizeX(); ++x)
		{
		u32 incOffset = imageSize - rowSize - (y * rowSize) + (x * 3);
		flipped[y * rowSize + (x * 3)] = screenshot[incOffset];
		flipped[y * rowSize + (x * 3) + 1] = screenshot[incOffset + 1];
		flipped[y * rowSize + (x * 3) + 2] = screenshot[incOffset + 2];
		}
		}*/

		//SOIL_save_image(filepath.c_str(), SOIL_SAVE_TYPE_BMP, getSizeX(), getSizeY(), 3, flipped);

		FILE *f;
		//unsigned char *img = NULL;
		int filesize = 54 + imageSize;  //w is your image width, h is image height, both int
										//if (img)
										//free(img);
										//img = (unsigned char *)malloc(3 * getSizeX() * getSizeY());
										//memset(img, 0, sizeof(img));

										/*for (int i = 0; i<w; i++)
										{
										for (int j = 0; j<h; j++)
										{
										x = i; y = (h - 1) - j;
										r = red[i][j] * 255;
										g = green[i][j] * 255;
										b = blue[i][j] * 255;
										if (r > 255) r = 255;
										if (g > 255) g = 255;
										if (b > 255) b = 255;
										img[(x + y*w) * 3 + 2] = (unsigned char)(r);
										img[(x + y*w) * 3 + 1] = (unsigned char)(g);
										img[(x + y*w) * 3 + 0] = (unsigned char)(b);
										}
										}*/

		unsigned char bmpfileheader[14] = { 'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0 };
		unsigned char bmpinfoheader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0 };
		unsigned char bmppad[3] = { 0,0,0 };

		bmpfileheader[2] = (unsigned char)(filesize);
		bmpfileheader[3] = (unsigned char)(filesize >> 8);
		bmpfileheader[4] = (unsigned char)(filesize >> 16);
		bmpfileheader[5] = (unsigned char)(filesize >> 24);

		int w = getSizeX(); int h = getSizeY();

		bmpinfoheader[4] = (unsigned char)(w);
		bmpinfoheader[5] = (unsigned char)(w >> 8);
		bmpinfoheader[6] = (unsigned char)(w >> 16);
		bmpinfoheader[7] = (unsigned char)(w >> 24);
		bmpinfoheader[8] = (unsigned char)(h);
		bmpinfoheader[9] = (unsigned char)(h >> 8);
		bmpinfoheader[10] = (unsigned char)(h >> 16);
		bmpinfoheader[11] = (unsigned char)(h >> 24);

		auto er = fopen_s(&f, filepath.c_str(), "wb");
		fwrite(bmpfileheader, 1, 14, f);
		fwrite(bmpinfoheader, 1, 40, f);

		fwrite(screenshot, 3, w * h, f);

		/*for (int i = 0; i<h; i++)
		{
		fwrite(flipped + (w*(h - i - 1) * 3), 3, w, f);
		fwrite(bmppad, 1, (4 - (w * 3) % 4) % 4, f);
		}*/
		fclose(f);

		/*SDL_Surface * surface = SDL_CreateRGBSurfaceFrom(flipped, getSizeX(), getSizeY(), 8 * 3, getSizeX() * 3, 0, 0, 0, 0);
		SDL_SaveBMP(surface, filepath.c_str());
		SDL_FreeSurface(surface);*/
		delete[] screenshot;
		//delete[] flipped;
	}

	inline s32 getSizeX() const { return actualClientArea.x; }
	inline s32 getSizeY() const { return actualClientArea.y; }
	inline s32 getPosX() const { return posX; }
	inline s32 getPosY() const { return posY; }
	inline glm::ivec2 getSize() const { return glm::ivec2(actualClientArea); }

	//private:

	bool windowCreated;
	//s32 sizeX, sizeY;
	s32 posX, posY;
	//SDL_Window* sdlWindow;
	//SDL_GLContext glContext;

	HGLRC glContextHandle;
	HWND windowHandle;
	WNDCLASS windowClass;
	HDC deviceContextHandle;
	KeyboardState keyboard;
	MouseState mouse;
	bool windowHasFocus;
	glm::ivec2 desiredClientArea;
	glm::ivec2 actualClientArea;
};


