#pragma once
#include "Window.h"
#include "Engine.h"
#include "Camera.h"
#include "Renderer.h"
#include "Keyboard.h"
#include "Event.h"

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

	setResolution(glm::ivec2(pWidth, pHeight));
	createGLContext();

	ShowWindow(windowHandle, 1);

	std::cout << "Screen Resolution: " << desiredClientArea.x << "x" << desiredClientArea.y << std::endl;

	//SWAP INTERVAL VSYNC
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

void Window::forceClientAreaToDesired()
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
		int x = 10;
	}
}

void Window::screenshot(StringGeneric& fileName)
{
	u32 imageSize = getSizeX() * getSizeY() * 3;
	u32 rowSize = getSizeX() * 3;
	u8* screenshot = new u8[imageSize];

	//String128 filepath = String128("screenshot/");
	fileName.append(String128(".bmp"));
	fileName.append('\0');
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glReadBuffer(GL_BACK);
	glReadPixels(0, 0, getSizeX(), getSizeY(), GL_BGR, GL_UNSIGNED_BYTE, screenshot);


	FILE *f;
	int filesize = 54 + imageSize;

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

	auto er = fopen_s(&f, fileName.getString(), "wb");
	fwrite(bmpfileheader, 1, 14, f);
	fwrite(bmpinfoheader, 1, 40, f);

	fwrite(screenshot, 3, w * h, f);


	fclose(f);

	delete[] screenshot;
}

void Window::screenshot()
{
	time_t now = time(0);
	tm  tstruct;
	char buf[80];
	localtime_s(&tstruct, &now);
	//OutputDebugString("SCREENSHOT");
	strftime(buf, sizeof(buf), "%F_%H;%M;%S", &tstruct);
	String128 fileName = String128("screenshot/Screenshot_").append(String64(buf));
	screenshot(fileName);
}

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//Window* me = (Window*)(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	//if (me) return me->realWndProc(hWnd, message, wParam, lParam);
	//return DefWindowProc(hWnd, message, wParam, lParam);

	int i, g, a;
	switch (message)
	{
	case WM_SIZE:
	{
		PWINDOWPOS data = PWINDOWPOS(lParam);
		//SetWindowPos(engineWindow->windowHandle, HWND_TOP, data->x, data->y, data->cx, data->cy, data->flags);
		return 0;
	}
	case WM_KEYDOWN:
	{
		engineWindow->keyboard.keyState[wParam] = false;
		auto check = [](u8 pKey) -> bool { return engineWindow->keyboard.isKeyPressed(pKey); };
		Event newEvent(Event::KeyDown); newEvent.constructKey(wParam, check(KeyCode::KC_SHIFT), check(KeyCode::KC_ALT), check(KeyCode::KC_SYS), check(KeyCode::KC_CAPS), check(KeyCode::KC_CTRL));
		engineWindow->eventQ.pushEvent(newEvent);
		break;
	}
	case WM_KEYUP:
	{
		engineWindow->keyboard.keyState[wParam] = true;
		auto check = [](u8 pKey) -> bool { return engineWindow->keyboard.isKeyPressed(pKey); };
		Event newEvent(Event::KeyUp); newEvent.constructKey(wParam, check(KeyCode::KC_SHIFT), check(KeyCode::KC_ALT), check(KeyCode::KC_SYS), check(KeyCode::KC_CAPS), check(KeyCode::KC_CTRL));
		engineWindow->eventQ.pushEvent(newEvent);
		break;
	}
	case WM_INPUT:
	{
		UINT dwSize;

		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize,
		sizeof(RAWINPUTHEADER));
		LPBYTE lpb = new BYTE[dwSize];
		if (lpb == NULL)
		{
		return 0;
		}

		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
			OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

		RAWINPUT* raw = (RAWINPUT*)lpb;

		STRSAFE_LPSTR szTempOutput = new char[1000];
		HRESULT hResult;

		if (raw->header.dwType == RIM_TYPEKEYBOARD)
		{

		}
		else if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			if (engineWindow->hasFocus())
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

				glm::ivec2 mPos; mPos = engineWindow->mouse.getWindowPosition(engineWindow);

				if (lMouseClick){
					engineWindow->mouse.leftDown = true;
					Event newEvent(Event::MouseDown); newEvent.constructMouse(0, mPos, 0);
					engineWindow->eventQ.pushEvent(newEvent);
					std::string mp;
					mp = std::to_string(engineWindow->mouse.getWindowPosition(engineWindow).x)  + " : " + std::to_string(engineWindow->mouse.getWindowPosition(engineWindow).y) + '\n';
				}
				else if (lMouseRelease){
					engineWindow->mouse.leftDown = false;
					Event newEvent(Event::MouseUp); newEvent.constructMouse(0, mPos, 0);
					engineWindow->eventQ.pushEvent(newEvent);
				}

				if (rMouseClick){
					engineWindow->mouse.rightDown = true;
					Event newEvent(Event::MouseDown); newEvent.constructMouse(1, mPos, 0);
					engineWindow->eventQ.pushEvent(newEvent);
					//TODO: IF MOUSE DISAPPEAR WHEN CLICKED
					ShowCursor(false);
				}
				else if (rMouseRelease){
					engineWindow->mouse.rightDown = false;
					Event newEvent(Event::MouseUp); newEvent.constructMouse(1, mPos, 0);
					engineWindow->eventQ.pushEvent(newEvent);
					ShowCursor(true);
				}

				if (mMouseClick) {
					engineWindow->mouse.middleDown = true;
				}
				else if (mMouseRelease) {
					engineWindow->mouse.middleDown = false;
				}

				glm::ivec2 mouseDelta;
				mouseDelta.x = raw->data.mouse.lLastX;
				mouseDelta.y = raw->data.mouse.lLastY;

				if (engineWindow->mouse.rightDown)
				{
					const float mouseX_Sensitivity = 0.004f;
					const float mouseY_Sensitivity = 0.004f;

					Engine::cam.targetYaw += mouseX_Sensitivity * mouseDelta.x;
					Engine::cam.targetPitch += mouseY_Sensitivity * mouseDelta.y;

					glm::quat rotation(glm::angleAxis(Engine::cam.targetPitch, glm::vec3(1.0f, 0.0f, 0.0f)));
					rotation = rotation * glm::angleAxis(Engine::cam.targetYaw, glm::vec3(0.0f, 1.0f, 0.0f));

					Engine::cam.qRot = rotation;
				}
				
				engineWindow->mouse.setDelta(mouseDelta);

				//Engine::select();
			}
			else
			{
				const auto anyMouseClick = RI_MOUSE_LEFT_BUTTON_DOWN | RI_MOUSE_RIGHT_BUTTON_DOWN | RI_MOUSE_MIDDLE_BUTTON_DOWN;
				if (raw->data.mouse.usButtonFlags | anyMouseClick == anyMouseClick && raw->data.mouse.usButtonFlags != 0)
				{
					if (engineWindow->isMouseInClientArea())
					{
						
						engineWindow->captureMouseFocus();
					}
				}
			}
		}

		delete[] lpb;
		return 0;
	}
	case WM_COMMAND:
	{

		break;
	}
	case WM_ACTIVATE:
	{
		if (wParam != 0)
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
			//ShowCursor(true);
			//OutputDebugString("SHOW CURSOR\n");
		}
		break;
	}
	case WM_KILLFOCUS:
	{
		//PAUSE
		engineWindow->windowHasFocus = false;
		//ShowCursor(true);
		break;
	}
	case WM_WINDOWPOSCHANGED:
	{
		PWINDOWPOS data = PWINDOWPOS(lParam);
		engineWindow->posX = data->x;
		engineWindow->posY = data->y;
		
		//SetWindowPos(engineWindow->windowHandle, HWND_TOP, data->x, data->y, data->cx, data->cy, data->flags);

		break;
	}
	case WM_CREATE:
	{
		engineWindow->setUpNewWindow();
		engineWindow->registerInputDevices();
		break;
	}
	case WM_DESTROY:
	{
		engineWindow->deleteWindow();
		//PostQuitMessage(0);
		//Engine::stop();
		SendMessage(hWnd, WM_QUIT, wParam, lParam);
		//DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	case WM_QUIT:
	{
		Engine::stop();
		//DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	case WM_CLOSE:
	{
		DestroyWindow(engineWindow->windowHandle);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}