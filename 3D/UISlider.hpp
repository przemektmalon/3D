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
	}

	void draw()
	{
		range.setProj(parentWindow->getProj());
		slider.setProj(parentWindow->getProj());

		range.draw();
		slider.draw();
	}

	void onMouseDown(MouseEvent& pMouseEvent)
	{
		if (onMouseDownImpl)
			onMouseDownImpl(parentWindow, this, pMouseEvent);

		auto uiMousePos = pMouseEvent.getUIWindowPosition(getParentWindow());
		auto mp = glm::ivec2(uiMousePos.x, getParentWindow()->getWindowRect().height - uiMousePos.y);

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

	glm::fvec4 sliderColour;
	glm::fvec4 sliderClickColour;
	glm::fvec4 sliderHoverColour;
	glm::fvec4 rangeColour;

	bool isSliding;

	RectangleShape slider;
	RectangleShape range;
	Text2D leftLimit;
	Text2D rightLimit;
};

