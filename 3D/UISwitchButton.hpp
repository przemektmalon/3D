#pragma once
#include "UIElement.hpp"
#include "UIButton.hpp"
#include "UIRectangleShape.hpp"

class UIWindow;

class UISwitchButton : public UIElement
{
public:
	UISwitchButton(UIWindow* pParent);
	~UISwitchButton();

	void draw()
	{
		button.setProj(parentWindow->getProj());
		backLeft.setProj(parentWindow->getProj());
		backRight.setProj(parentWindow->getProj());

		button.draw();
		backLeft.draw();
		backRight.draw();
	}

	void onMouseDown(MouseEvent& pMouseEvent)
	{
		if (onMouseDownImpl)
			onMouseDownImpl(parentWindow, this, pMouseEvent);
	}

	void onMouseUp(MouseEvent& pMouseEvent)
	{
		if (onMouseUpImpl)
			onMouseUpImpl(parentWindow, this, pMouseEvent);

		on = !on;

		if (on)
		{
			auto newBounds = button.getBounds();
			newBounds.left = backRight.getBounds().left;
			button.setBounds(newBounds);
		}
		else
		{
			auto newBounds = button.getBounds();
			newBounds.left = backLeft.getBounds().left;
			button.setBounds(newBounds);
		}

		if (isHovered())
			button.setColour(buttonBackHoverColour);
		else
			button.setColour(buttonBackColour);
	}

	void onMouseEnter(MouseEvent& pMouseEvent)
	{
		if (onMouseEnterImpl)
			onMouseEnterImpl(parentWindow, this, pMouseEvent);

		button.setColour(buttonBackHoverColour);
	}

	void onMouseLeave(MouseEvent& pMouseEvent)
	{
		if (onMouseLeaveImpl)
			onMouseLeaveImpl(parentWindow, this, pMouseEvent);

		button.setColour(buttonBackColour);
	}

	glm::fvec4 buttonBackColour;
	glm::fvec4 buttonBackHoverColour;
	glm::fvec4 offColour;
	glm::fvec4 onColour;

	bool on;

	RectangleShape button;
	RectangleShape backLeft;
	RectangleShape backRight;
};

