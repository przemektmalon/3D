#include "UIElement.hpp"



UIElement::UIElement(ElementType elType, UIWindow* pParent) : elementType(elType), parentWindow(pParent)
{
}


UIElement::~UIElement()
{
}
