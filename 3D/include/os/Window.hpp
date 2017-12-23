#pragma once
#include "Types.hpp"
#include <iostream>
#include <string>
#include <sstream>

#include <GL\glew.h>
#include <GL\wglew.h>
#include "MathIncludes.hpp"
#include <time.h>
#include <Windows.h>
#include <SDKDDKVer.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <strsafe.h>

#include "Keyboard.hpp"
#include "Mouse.hpp"

#include "Event.hpp"

class Window
{
public:
	Window() : windowCreated(false) {}
	~Window() {}

	static Window* engineWindow;

	BOOL processMessages();
	void postMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		PostMessage(windowHandle, message, wParam, lParam);
	}

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void deleteWindow();
	void createWindow(HINSTANCE pHInstance, LPCTSTR pWindowName, int pXPos, int pYPos, int pWidth, int pHeight, DWORD pWindowStyle);
	void createGLContext();

	bool hasFocus() {
		return windowHasFocus; }

	void escapePressed() {
		PostMessage(windowHandle, WM_CLOSE, 0, 0); }

	void setResolution(glm::ivec2 pRes){
		desiredClientArea = pRes;
		forceClientAreaToDesired();
		orthoProj = glm::ortho(0.f, (float)pRes.x, (float)pRes.y, 0.f, -1.f, 100.f); }

	RECT getClientArea(){
		RECT wr2;
		auto er2 = GetClientRect(windowHandle, &wr2); return wr2; }

	void forceClientAreaToDesired();
	bool isMouseInClientArea();
	void captureMouseFocus(){
		windowHasFocus = true;}

	void registerInputDevices();
	void screenshot();
	void screenshot(std::string fileName);

	inline void swapBuffers() const {
		wglSwapLayerBuffers(deviceContextHandle, WGL_SWAP_MAIN_PLANE);}

	inline s32 getSizeX() const { return actualClientArea.x; }
	inline s32 getSizeY() const { return actualClientArea.y; } 
	inline s32 getPosX() const { return posX; }
	inline s32 getPosY() const { return posY; }
	inline glm::ivec2 getSize() const { return glm::ivec2(actualClientArea); }
	inline glm::fmat4 getOrthoProj() const { return orthoProj; }

	glm::ivec2 getMousePosition()
	{
		return mouse.getWindowPosition(this);
	}

//private:

	bool windowCreated;
	
	glm::fmat4 orthoProj;

	glm::ivec2 desiredClientArea;
	glm::ivec2 actualClientArea;
	s32 posX, posY;

	HGLRC glContextHandle;
	HWND windowHandle;
	WNDCLASS windowClass;
	HDC deviceContextHandle;

	Keyboard keyboard;
	Mouse mouse;
	
	EventQ eventQ;

	bool windowHasFocus;
};