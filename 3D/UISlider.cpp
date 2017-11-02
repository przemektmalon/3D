#include "UISlider.hpp"
#include "UIWindow.hpp"
#include "Engine.hpp"
#include "Window.hpp"

UISlider::UISlider(UIWindow* pParent) : UIElement(Slider,pParent)
{
	slider.initOGL();
	range.initOGL();
	leftLimit.init();
	rightLimit.init();

	sliderColour = glm::fvec4(0.7, 0.7, 0.7, 0.8);
	sliderClickColour = glm::fvec4(0.8, 0.8, 0.8, 0.9);
	sliderHoverColour = glm::fvec4(0.75, 0.75, 0.75, 0.8);
	rangeColour = glm::fvec4(0.3, 0.3, 0.3, 0.9);

	slider.setColour(sliderColour);
	range.setColour(rangeColour);

	setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UISlider* _this = (UISlider*)__this;
		if (_this->isSliding)
		{
			auto newBounds = _this->slider.getBounds();
			newBounds.left = Engine::window.mouse.getWindowPosition(&Engine::window).x - 4;
			if (newBounds.left < _this->range.getBounds().left)
				newBounds.left = _this->range.getBounds().left;
			if (newBounds.right() > _this->range.getBounds().right())
				newBounds.left = _this->range.getBounds().left + _this->range.getBounds().width;
			_this->slider.setBounds(newBounds);
		}
	});
}


UISlider::~UISlider()
{

}

void UISlider::update()
{
	updateImpl(parentWindow,this);
}
