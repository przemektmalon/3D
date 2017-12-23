#include "UILabel.hpp"



UILabel::UILabel(UIWindow* pParent) : UIElement(Label, pParent)
{
	text.init();
	setWindowOrigin(Text2D::TopLeft);
}


UILabel::~UILabel()
{
}
