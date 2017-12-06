#include "UIMultiTab.hpp"
#include "Engine.hpp"
#include "AssetManager.hpp"

UIMultiTab::UIMultiTab(UIWindow* pParent) : UIElement(MultiTab, pParent)
{
	back.initOGL();
	back.setColour(glm::fvec4(0.7, 0.7, 0.7, 0.7));

	setOnMouseDown([](UIWindow* win, UIElement* __this, MouseEvent& pMouseEvent) -> void {
		UIMultiTab* _this = (UIMultiTab*)__this;
		for (auto itr = _this->tabButtons.begin(); itr != _this->tabButtons.end(); ++itr)
		{
			auto rect = (itr)->second->getBounds();

			auto winMP = pMouseEvent.getUIWindowPosition((itr)->second->getParentWindow());

			if (!rect.contains(winMP))
				continue;

			(itr)->second->setFocused(true);
			(itr)->second->setClicked(true);
			(itr)->second->onMouseDown(pMouseEvent);
		}

		if (_this->tabElements.find(_this->currentTab) == _this->tabElements.end())
			return;

		_this->tabElements[_this->currentTab].mouseDown(pMouseEvent);
	});

	setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& pMouseEvent) -> void {
		UIMultiTab* _this = (UIMultiTab*)__this;
		for (auto itr = _this->tabButtons.begin(); itr != _this->tabButtons.end(); ++itr)
		{
			if ((itr)->second->isClicked())
			{
				(itr)->second->setClicked(false);
				(itr)->second->onMouseUp(pMouseEvent);
			}
		}

		if (_this->tabElements.find(_this->currentTab) == _this->tabElements.end())
			return;

		_this->tabElements[_this->currentTab].mouseUp(pMouseEvent);
	});

	setOnKeyboardDown([](UIWindow* win, UIElement* __this, KeyEvent& pKeyEvent) -> void {
		UIMultiTab* _this = (UIMultiTab*)__this;
		if (_this->tabElements.find(_this->currentTab) == _this->tabElements.end())
			return;

		_this->tabElements[_this->currentTab].keyDown(pKeyEvent);
	});

	setOnKeyboardUp([](UIWindow* win, UIElement* __this, KeyEvent& pKeyEvent) -> void {
		UIMultiTab* _this = (UIMultiTab*)__this;
		if (_this->tabElements.find(_this->currentTab) == _this->tabElements.end())
			return;

		_this->tabElements[_this->currentTab].keyUp(pKeyEvent);
	});

	setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UIMultiTab* _this = (UIMultiTab*)__this;
		for (auto& but : _this->tabButtons)
			but.second->update();

		if (_this->tabElements.find(_this->currentTab) == _this->tabElements.end())
			return;

		_this->tabElements[_this->currentTab].update();
	});

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