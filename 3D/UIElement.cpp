#include "UIElement.h"



UIElement::UIElement(ElementType elType, UIWindow* pParent) : elementType(elType), parentWindow(pParent)
{
}


UIElement::~UIElement()
{
}
