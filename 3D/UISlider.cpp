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
}

UISlider::~UISlider()
{

}

void UISlider::update()
{
	if (isSliding)
	{
		auto newBounds = slider.getBounds();
		newBounds.left = Engine::window.mouse.getWindowPosition(&Engine::window).x - 4 - parentWindow->getWindowArea().left;
		if (newBounds.left < range.getBounds().left)
			newBounds.left = range.getBounds().left;
		if (newBounds.right() > range.getBounds().right())
			newBounds.left = range.getBounds().left + range.getBounds().width - slider.getBounds().width;
		slider.setBounds(newBounds);

		float ratio = float(newBounds.left - range.getBounds().left) / float(range.getBounds().width - slider.getBounds().width);

		std::string valString;

		switch (valType)
		{
		case Integer:
			value.i = limits.left.i + int(float(limits.right.i - limits.left.i) * ratio);
			valString = std::to_string(value.i);
			if (binding.i)
				*binding.i = value.i;
			break;
		case Float:
			value.f = limits.left.f + (float(limits.right.f - limits.left.f) * ratio);
			valString = std::to_string(value.f);
			if (binding.f)
				*binding.f = value.f;
			break;
		}

		valueText.setString(valString);
		valueText.forceUpdate();
	}

	if (updateImpl)
		updateImpl(parentWindow,this);
}
