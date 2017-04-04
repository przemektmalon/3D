#pragma once
#include "UIElement.h"

class UIWindow;

class UISlider :
	public UIElement
{
public:
	UISlider(UIWindow* pParent);
	~UISlider();
};

