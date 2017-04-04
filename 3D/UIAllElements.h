#pragma once
#include "UIButton.h"
#include "UILabel.h"
#include "UISlider.h"
#include "UISwitchButton.h"
#include "UIUtil.h"

#define C_LABEL(name,window) \
UILabel* ##name##Ptr = new UILabel(window); \
UILabel& ##name## = *##name##Ptr;

#define C_BUTTON(name,window) \
UIButton* ##name##Ptr = new UIButton(window); \
UIButton& ##name## = *##name##Ptr;

#define C_SLIDER(name,window) \
UISlider* ##name##Ptr = new UISlider(window); \
UISlider& ##name## = *##name##Ptr;