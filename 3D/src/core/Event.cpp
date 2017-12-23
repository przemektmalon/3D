#include "Event.hpp"
#include "UIWindow.hpp"

glm::ivec2 MouseEvent::getUIWindowPosition(UIWindow* uiw)
{
	/*glm::ivec2 nm = mouse.position;
	nm.y = uiw->parentWindow->getSizeY() - nm.y;
	nm.y = nm.y - uiw->getWindowArea().top;
	nm.x = nm.x - uiw->getWindowArea().left;

	return nm;*/
	return mouse.position - glm::ivec2(uiw->getWindowArea().left, uiw->getWindowArea().top);
}