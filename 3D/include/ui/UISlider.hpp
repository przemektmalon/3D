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

	void init(glm::fvec2 pPosition, float pWidth)
	{
		position = pPosition;
		size.x = pWidth; size.y = 40;

		descText.setPosition(glm::fvec2(position.x + 10, position.y));
		
		range.setBounds(irect(position.x, position.y + 25, size.x, 10));
		valueText.setPosition(glm::fvec2(position.x + size.x - 10, position.y));

		float ratio;

		std::string valString;

		switch (valType)
		{
		case Integer:
			ratio = float(value.i - limits.left.i) / float(limits.right.i - limits.left.i);
			value.i = limits.left.i + int(float(limits.right.i - limits.left.i) * ratio);
			valString = std::to_string(value.i);
			if (binding.i)
				*binding.i = value.i;
			break;
		case Float:
			ratio = (value.f - limits.left.f) / (limits.right.f - limits.left.f);
			value.f = limits.left.f + (float(limits.right.f - limits.left.f) * ratio);
			valString = std::to_string(value.f);
			if (binding.f)
				*binding.f = value.f;
			break;
		}

		slider.setBounds(irect(position.x + (float(size.x) * ratio), position.y + 17, 12, 24));

		valueText.setString(valString);
		valueText.forceUpdate();
	}

	void draw()
	{
		range.setProj(parentWindow->getProj());
		slider.setProj(parentWindow->getProj());

		range.draw();
		slider.draw();

		descText.shader->setProj(parentWindow->getProj());
		descText.shader->setModel(glm::fmat4());
		descText.shader->setView(glm::fmat4());
		descText.shader->setFontBinding(12);
		descText.draw();

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

	void setDescription(std::string pDesc)
	{
		descText.setString(pDesc);
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

	union Binding
	{
		float* f;
		s32* i;
	}binding;

	Text2D valueText;
	Text2D descText;
};

