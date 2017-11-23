#pragma once
#include "Window.hpp"
#include "Engine.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"
#include "Keyboard.hpp"
#include "Event.hpp"

Window* Window::engineWindow;


BOOL Window::processMessages()
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

void Window::deleteWindow()
{
	if (windowCreated)
	{
		wglDeleteContext(glContextHandle);
		DeleteDC(deviceContextHandle);
		windowCreated = false;
	}
}

void Window::createWindow(HINSTANCE pHInstance, LPCTSTR pWindowName, int pXPos, int pYPos, int pWidth, int pHeight, DWORD pWindowStyle = 0)
{
	if (windowCreated)
		return;

	windowCreated = true;

	engineWindow = const_cast<Window*>(this);
	windowClass.lpfnWndProc = WndProc;
	windowClass.hInstance = pHInstance;
	windowClass.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	windowClass.lpszClassName = LPCSTR(pWindowName);
	windowClass.style = CS_OWNDC;
	if (!RegisterClass(&windowClass))
		std::cout << "Error: window class not registered" << std::endl;

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

	windowHandle = CreateWindowEx(0, LPCSTR(windowClass.lpszClassName), pWindowName, pWindowStyle, pXPos, pYPos, pWidth, pHeight, 0, 0, pHInstance, 0);

	RECT ca = getClientArea();
	actualClientArea.x = ca.right;
	actualClientArea.y = ca.bottom;

	setResolution(glm::ivec2(pWidth, pHeight));
	createGLContext();

	ShowWindow(windowHandle, 1);

	std::cout << "Screen Resolution: " << desiredClientArea.x << "x" << desiredClientArea.y << std::endl;
}

void Window::createGLContext()
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

	int  pixelFormat = ChoosePixelFormat(deviceContextHandle, &pfd);
	SetPixelFormat(deviceContextHandle, pixelFormat, &pfd);

	HGLRC dummyContext = wglCreateContext(deviceContextHandle);
	wglMakeCurrent(deviceContextHandle, dummyContext);

	typedef HGLRC(APIENTRY * PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC, HGLRC hShareContext, const int *attribList);
	static PFNWGLCREATECONTEXTATTRIBSARBPROC pfnCreateContextAttribsARB = 0;

	int attriblist[] = { 
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4, 
		WGL_CONTEXT_MINOR_VERSION_ARB, 5,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
		0, 0 };

	pfnCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
	auto newRenderContext = pfnCreateContextAttribsARB(deviceContextHandle, 0, attriblist);

	wglMakeCurrent(deviceContextHandle, 0);
	wglDeleteContext(dummyContext);

	wglMakeCurrent(deviceContextHandle, newRenderContext);

	glContextHandle = newRenderContext;

	std::cout << "GL context created" << std::endl;

	GLint maj, min;
	glGetIntegerv(GL_MAJOR_VERSION, &maj);
	glGetIntegerv(GL_MINOR_VERSION, &min);

	std::cout << "GL version " << maj << "." << min << std::endl;

	auto glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);
	std::cout << "GLSL version " << glsl << std::endl;
}

void Window::forceClientAreaToDesired()
{
	RECT wr;
	auto er = GetWindowRect(windowHandle, &wr);
	RECT wr2 = getClientArea();
	POINT mouse;

	if (wr2.right != desiredClientArea.x || wr2.bottom != desiredClientArea.y)
	{
		glm::ivec2 windowSpaceToAdd = desiredClientArea - glm::ivec2(wr2.right, wr2.bottom);
		RECT wr3;
		wr3.left = wr.left; wr3.top = wr.top;
		wr3.right = (wr.right - wr.left) + windowSpaceToAdd.x; wr3.bottom = (wr.bottom - wr.top) + windowSpaceToAdd.y;
		MoveWindow(windowHandle, wr3.left, wr3.top, wr3.right, wr3.bottom, 0);
	}
	actualClientArea.x = desiredClientArea.x;
	actualClientArea.y = desiredClientArea.y;
}

bool Window::isMouseInClientArea()
{
	RECT wr;
	auto er = GetWindowRect(windowHandle, &wr);
	RECT wr2;
	auto er2 = GetClientRect(windowHandle, &wr2);
	POINT mouse;

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

void Window::registerInputDevices()
{
	RAWINPUTDEVICE Rid[1];

	Rid[0].usUsagePage = 0x01;
	Rid[0].usUsage = 0x02;
	Rid[0].dwFlags = 0;
	Rid[0].hwndTarget = 0;

	if (!RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]))) {
		//registration failed. Call GetLastError for the cause of the error
		std::cout << "Error: mouse raw input not registered" << std::endl;
	}
}

void Window::screenshot(std::string fileName)
{
	u32 imageSize = getSizeX() * getSizeY() * 3;
	u32 rowSize = getSizeX() * 3;
	u8* screenshot = new u8[imageSize];

	fileName.append(".bmp");
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glReadBuffer(GL_BACK);
	glReadPixels(0, 0, getSizeX(), getSizeY(), GL_RGB, GL_UNSIGNED_BYTE, screenshot);

	SOIL_save_image(fileName.c_str(), SOIL_SAVE_TYPE_BMP, getSizeX(), getSizeY(), 3, screenshot);

	delete[] screenshot;
}

void Window::screenshot()
{
	time_t now = time(0);
	tm  tstruct;
	char buf[80];
	localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), "%F %H,%M,%S", &tstruct);
	screenshot("screenshot/" + std::string(buf));
}

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
	{
		engineWindow->keyboard.keyState[wParam] = true;
		auto check = [](u8 pKey) -> bool { return engineWindow->keyboard.isKeyPressed(pKey); };
		Event newEvent(Event::KeyDown); newEvent.constructKey(wParam, check(Key::KC_SHIFT), check(Key::KC_ALT), check(Key::KC_SYS), check(Key::KC_CAPS), check(Key::KC_CTRL));
		engineWindow->eventQ.pushEvent(newEvent);
		break;
	}
	case WM_KEYUP:
	{
		engineWindow->keyboard.keyState[wParam] = false;
		auto check = [](u8 pKey) -> bool { return engineWindow->keyboard.isKeyPressed(pKey); };
		Event newEvent(Event::KeyUp); newEvent.constructKey(wParam, check(Key::KC_SHIFT), check(Key::KC_ALT), check(Key::KC_SYS), check(Key::KC_CAPS), check(Key::KC_CTRL));
		engineWindow->eventQ.pushEvent(newEvent);
		break;
	}
	case WM_INPUT:
	{
		UINT dwSize;

		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));

		LPBYTE lpb = new BYTE[dwSize];
		
		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
			OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

		RAWINPUT* raw = (RAWINPUT*)lpb;

		if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			const auto buttonFlag = raw->data.mouse.usButtonFlags;

			const auto anyMouseClick = (RI_MOUSE_LEFT_BUTTON_DOWN | RI_MOUSE_RIGHT_BUTTON_DOWN | RI_MOUSE_MIDDLE_BUTTON_DOWN) & buttonFlag;
			const auto lMouseClick = RI_MOUSE_LEFT_BUTTON_DOWN & buttonFlag;
			const auto rMouseClick = RI_MOUSE_RIGHT_BUTTON_DOWN & buttonFlag;
			const auto mMouseClick = RI_MOUSE_MIDDLE_BUTTON_DOWN & buttonFlag;

			const auto anyMouseRelease = (RI_MOUSE_LEFT_BUTTON_UP | RI_MOUSE_RIGHT_BUTTON_UP | RI_MOUSE_MIDDLE_BUTTON_UP) & buttonFlag;
			const auto lMouseRelease = RI_MOUSE_LEFT_BUTTON_UP & buttonFlag;
			const auto rMouseRelease = RI_MOUSE_RIGHT_BUTTON_UP & buttonFlag;
			const auto mMouseRelease = RI_MOUSE_MIDDLE_BUTTON_UP & buttonFlag;

			if (engineWindow->hasFocus())
			{
				glm::ivec2 mPos; mPos = engineWindow->mouse.getWindowPosition(engineWindow);

				if (lMouseClick)
					engineWindow->mouse.state |= Mouse::M_LEFT;
				else if (lMouseRelease)
					engineWindow->mouse.state &= ~Mouse::M_LEFT;

				if (rMouseClick)
					engineWindow->mouse.state |= Mouse::M_RIGHT;
				else if (rMouseRelease)
					engineWindow->mouse.state &= ~Mouse::M_RIGHT;

				if (mMouseClick)
					engineWindow->mouse.state |= Mouse::M_MIDDLE;
				else if (mMouseRelease)
					engineWindow->mouse.state &= ~Mouse::M_MIDDLE;

				MouseCode mouseCode = engineWindow->mouse.state;

				Event::Type eventType;

				if (anyMouseClick)
				{
					eventType = Event::MouseDown;
					Event mouseEvent(eventType);
					mouseEvent.constructMouse(mouseCode, mPos, 0);
					engineWindow->eventQ.pushEvent(mouseEvent);		// Send mouse down event
				}
				else if (anyMouseRelease)
				{
					eventType = Event::MouseUp;
					Event mouseEvent(eventType);
					mouseEvent.constructMouse(mouseCode, mPos, 0);
					engineWindow->eventQ.pushEvent(mouseEvent);		// Send mouse up event
				}

				glm::ivec2 mouseMove;
				mouseMove.x = raw->data.mouse.lLastX;
				mouseMove.y = raw->data.mouse.lLastY;

				if (mouseMove != glm::ivec2(0, 0))
				{
					Event moveEvent(Event::MouseMove);
					moveEvent.constructMouse(mouseCode, mPos, 0);
					moveEvent.mouse.move = mouseMove;
					engineWindow->eventQ.pushEvent(moveEvent);		// Send mouse move event
				}
			}
			else
			{
				if (anyMouseClick || engineWindow->isMouseInClientArea())
					engineWindow->captureMouseFocus();
			}
		}

		delete[] lpb;
		break;
	}
	case WM_ACTIVATE:
	{
		if (wParam == WA_ACTIVE || wParam == WA_CLICKACTIVE)
		{
			RECT wr;
			auto er = GetWindowRect(engineWindow->windowHandle, &wr);
			RECT wr2;
			auto er2 = GetClientRect(engineWindow->windowHandle, &wr2);
			POINT mouse;

			GetCursorPos(&mouse);

			RECT clientRect;
			clientRect.left = wr.right - wr2.right;
			clientRect.top = wr.bottom - wr2.bottom;
			clientRect.right = clientRect.left + wr2.right;
			clientRect.bottom = clientRect.top + wr2.bottom;

			if (mouse.y > clientRect.top && mouse.y < clientRect.bottom && mouse.x > clientRect.left && mouse.x < clientRect.right)
			{
				engineWindow->captureMouseFocus();
			}		
		}
		else
		{
			engineWindow->windowHasFocus = false;
		}
		break;
	}
	case WM_KILLFOCUS:
	{
		engineWindow->windowHasFocus = false;
		break;
	}
	case WM_WINDOWPOSCHANGED:
	{
		PWINDOWPOS data = PWINDOWPOS(lParam);
		engineWindow->posX = data->x;
		engineWindow->posY = data->y;
		break;
	}
	case WM_CREATE:
	{
		engineWindow->registerInputDevices();
		SetActiveWindow(engineWindow->windowHandle);
		break;
	}
	case WM_DESTROY:
	{
		// Delete the window

		engineWindow->deleteWindow();
		SendMessage(hWnd, WM_QUIT, wParam, lParam); // Quit the engine

		break;
	}
	case WM_QUIT:
	{
		// Quit the engine
		Engine::stop();
		break;
	}
	case WM_CLOSE:
	{
		// Close the window
		DestroyWindow(engineWindow->windowHandle);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}