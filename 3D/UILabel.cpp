#include "UILabel.h"



UILabel::UILabel(UIWindow* pParent) : UIElement(Label, pParent)
{
	text.init();
	setWindowOrigin(Text2D::TopLeft);
	setWindowSize(glm::ivec2(pParent->getWindowArea().width, pParent->getWindowArea().height));
}


UILabel::~UILabel()
{
}
