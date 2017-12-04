#include "UIMultiTab.hpp"
#include "Engine.hpp"
#include "AssetManager.hpp"

UIMultiTab::UIMultiTab(UIWindow* pParent) : UIElement(MultiTab, pParent)
{
	back.initOGL();
	back.setColour(glm::fvec4(0.7, 0.7, 0.7, 0.7));
}

void UIMultiTab::addTab(std::string name)
{
	auto but = new UIButton(parentWindow);
	tabButtons.insert(std::make_pair(name, but));
	currentTab = name;
	
	Text2D::TextStyle styleA(Engine::assets.getFont("clearsansb"), 12);

	but->setName(name);
	but->getText().setStyle(styleA);
	but->setString(name);
	but->getText().setTextOrigin(Text2D::MiddleMiddle);
	auto bounds = but->getText().getBoundingBox();
	but->setSize(glm::fvec2(bounds.width + 4, 18));
	but->setOnMouseUp([this](UIWindow* win, UIElement* __this, MouseEvent& me) -> void {
		UIButton* _this = (UIButton*)__this;
		((UIMultiTab*)win->getElement("tab"))->currentTab = _this->getName();
	});

	glm::fvec2 pos(maxRightTab + 5, position.y);

	but->setPosition(pos);
	maxRightTab = but->getBounds().right();
}