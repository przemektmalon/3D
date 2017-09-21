#include "Event.hpp"
#include "UIWindow.hpp"

glm::ivec2 MouseEvent::getUIWindowPosition(UIWindow* uiw)
{
	return mouse.position - glm::ivec2(uiw->getWindowArea().left, uiw->getWindowArea().top);
}