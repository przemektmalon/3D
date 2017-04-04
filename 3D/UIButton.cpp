#include "UIButton.h"



UIButton::UIButton(UIWindow* pParent) : UIElement(Button,pParent)
{
	text.init();
	rect.initOGL();
}


UIButton::~UIButton()
{
}
