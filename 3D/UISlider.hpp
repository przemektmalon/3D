#pragma once
#include "UIElement.hpp"
#include "Text.hpp"
#include "UIRectangleShape.hpp"

class UIWindow;

class UISlider : public UIElement
{
public:
	UISlider(UIWindow* pParent);
	~UISlider();

	void update();

	void init(irect pRangeBounds)
	{
		range.setBounds(pRangeBounds);
		slider.setBounds(irect(pRangeBounds.left, pRangeBounds.top - 8, 8, pRangeBounds.height + 16));
		position = glm::fvec2(pRangeBounds.left, pRangeBounds.top - 8);
		dimensions = glm::fvec2(pRangeBounds.width, pRangeBounds.height + 16);

		valueText.setPosition(glm::fvec2(position.x + (dimensions.width / 2.f), position.y - 16.f));

	}

	void draw()
	{
		range.setProj(parentWindow->getProj());
		slider.setProj(parentWindow->getProj());

		range.draw();
		slider.draw();

		valueText.shader->setProj(parentWindow->getProj());
		valueText.shader->setModel(glm::fmat4());
		valueText.shader->setView(glm::fmat4());
		valueText.shader->setFontBinding(12);
		valueText.draw();
	}

	void onMouseDown(MouseEvent& pMouseEvent)
	{
		if (onMouseDownImpl)
			onMouseDownImpl(parentWindow, this, pMouseEvent);

		auto uiMousePos = pMouseEvent.getUIWindowPosition(getParentWindow());
		auto mp = glm::ivec2(uiMousePos.x, uiMousePos.y);

		if (slider.getBounds().contains(mp))
		{
			isSliding = true;
			slider.setColour(sliderClickColour);
		}
	}

	void onMouseUp(MouseEvent& pMouseEvent)
	{
		if (onMouseUpImpl)
			onMouseUpImpl(parentWindow, this, pMouseEvent);

		isSliding = false;
		slider.setColour(sliderColour);
	}

	void onMouseEnter(MouseEvent& pMouseEvent)
	{
		if (onMouseEnterImpl)
			onMouseEnterImpl(parentWindow, this, pMouseEvent);

		slider.setColour(sliderHoverColour);
	}

	void onMouseLeave(MouseEvent& pMouseEvent)
	{
		if (onMouseLeaveImpl)
			onMouseLeaveImpl(parentWindow, this, pMouseEvent);

		slider.setColour(sliderColour);
	}

	void getLimits(glm::fvec2& ret)
	{
		ret.x = limits.left.f;
		ret.y = limits.right.f;
	}

	void getLimits(glm::ivec2& ret)
	{
		ret.x = limits.left.i;
		ret.y = limits.right.i;
	}

	void setLimits(glm::fvec2 l)
	{
		limits.left.f = l.x;
		limits.right.f = l.y;
		valueText.setString(std::to_string(l.x));
	}

	void setLimits(glm::ivec2 l)
	{
		limits.left.i = l.x;
		limits.right.i = l.y;
		valueText.setString(std::to_string(l.x));
	}

	glm::fvec4 sliderColour;
	glm::fvec4 sliderClickColour;
	glm::fvec4 sliderHoverColour;
	glm::fvec4 rangeColour;

	bool isSliding;

	RectangleShape slider;
	RectangleShape range;

	enum ValueType { Integer, Float };
	ValueType valType;
	struct Limits
	{
		union Left {
			float f;
			s32 i;
		} left;
		union Right {
			float f;
			s32 i;
		} right;
	} limits;

	union Value
	{
		float f;
		s32 i;
	}value;

	Text2D valueText;
};

