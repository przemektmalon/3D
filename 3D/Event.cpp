#include "Event.h"
#include "UIWindow.h"

glm::ivec2 MouseEvent::getUIWindowPosition(UIWindow* uiw)
{
	return mouse.position - glm::ivec2(uiw->getWindowArea().left, uiw->getWindowArea().top);
}