#include "UserInputManager.h"
#include "Engine.h"
#include "Window.h"

ControlArray UIM::defCA;

void escapePress()
{
	Engine::window.escapePressed();
}

void screenshot()
{
	Engine::window.screenshot();
}