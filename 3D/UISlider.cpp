#include "UISlider.hpp"
#include "UIWindow.hpp"
#include "Engine.hpp"
#include "Window.hpp"
#include "AssetManager.hpp"

UISlider::UISlider(UIWindow* pParent) : UIElement(Slider,pParent)
{
	slider.initOGL();
	range.initOGL();
	valueText.init();
	descText.init();

	Text2D::TextStyle style(Engine::assets.getFont("clearsansb"), 14);

	valueText.setStyle(style);
	valueText.setTextOrigin(Text2D::TopRight);

	descText.setStyle(style);
	descText.setTextOrigin(Text2D::TopLeft);

	sliderColour = glm::fvec4(0.7, 0.7, 0.7, 0.8);
	sliderClickColour = glm::fvec4(0.9, 0.9, 0.9, 0.9);
	sliderHoverColour = glm::fvec4(0.9, 1.0, 0.9, 0.8);
	rangeColour = glm::fvec4(0.3, 0.3, 0.3, 0.9);

	slider.setColour(sliderColour);
	range.setColour(rangeColour);

	binding.f = nullptr;

	setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UISlider* _this = (UISlider*)__this;
		if (_this->isSliding)
		{
			auto newBounds = _this->slider.getBounds();
			newBounds.left = Engine::window.mouse.getWindowPosition(&Engine::window).x - 4 - win->getWindowArea().left;
			if (newBounds.left < _this->range.getBounds().left)
				newBounds.left = _this->range.getBounds().left;
			if (newBounds.right() > _this->range.getBounds().right())
				newBounds.left = _this->range.getBounds().left + _this->range.getBounds().width - _this->slider.getBounds().width;
			_this->slider.setBounds(newBounds);

			float ratio = float(newBounds.left - _this->range.getBounds().left) / float(_this->range.getBounds().width - _this->slider.getBounds().width);

			std::string valString;

			switch (_this->valType)
			{
			case Integer:
				_this->value.i = _this->limits.left.i + int(float(_this->limits.right.i - _this->limits.left.i) * ratio);
				valString = std::to_string(_this->value.i);
				if (_this->binding.i)
					*_this->binding.i = _this->value.i;
				break;
			case Float:
				_this->value.f = _this->limits.left.f + (float(_this->limits.right.f - _this->limits.left.f) * ratio);
				valString = std::to_string(_this->value.f);
				if (_this->binding.f)
					*_this->binding.f = _this->value.f;
				break;
			}

			_this->valueText.setString(valString);
			_this->valueText.forceUpdate();
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
