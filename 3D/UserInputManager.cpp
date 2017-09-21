#include "UserInputManager.hpp"
#include "Engine.hpp"
#include "Window.hpp"

ControlArray UIM::defCA;

void escapePress()
{
	Engine::window.escapePressed();
}

void screenshot()
{
	Engine::window.screenshot();
}