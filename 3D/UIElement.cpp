#include "UIElement.hpp"
#include "Engine.hpp"
#include "Time.hpp"

UIElement::UIElement(ElementType elType, UIWindow* pParent) : elementType(elType), parentWindow(pParent), updateInterval(1.f/30.f), updateClock(0.f)
{
}


UIElement::~UIElement()
{
}

void UIElement::update()
{
	if (updateImpl != nullptr)
	{
		updateClock += Engine::deltaTime.getSecondsf();
		if (updateClock > updateInterval)
		{
			updateImpl(parentWindow, this);
			updateClock = 0.f;
		}
	}
}