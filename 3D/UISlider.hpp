#pragma once
#include "UIElement.hpp"

class UIWindow;

class UISlider :
	public UIElement
{
public:
	UISlider(UIWindow* pParent);
	~UISlider();
};

