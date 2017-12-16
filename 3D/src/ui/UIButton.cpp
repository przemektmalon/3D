#include "ui/UIButton.hpp"



UIButton::UIButton(UIWindow* pParent) : UIElement(Button,pParent)
{
	text.init();
	text.setTextOrigin(Text2D::MiddleMiddle);
	rect.initOGL();

	backColour = glm::fvec4(0.3, 0.3, 0.3, 0.8);
	backHoverColour = glm::fvec4(0.35, 0.35, 0.35, 0.8);
	backClickColour = glm::fvec4(0.6, 0.6, 0.6, 0.9);
	setRectColour(backColour);
}

UIButton::~UIButton()
{
}
