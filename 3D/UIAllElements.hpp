#pragma once
#include "UIButton.hpp"
#include "UILabel.hpp"
#include "UISlider.hpp"
#include "UISwitchButton.hpp"
#include "UIUtil.hpp"

#define C_LABEL(name,window) \
UILabel* ##name##Ptr = new UILabel(window); \
UILabel& ##name## = *##name##Ptr;

#define C_BUTTON(name,window) \
UIButton* ##name##Ptr = new UIButton(window); \
UIButton& ##name## = *##name##Ptr;

#define C_SLIDER(name,window) \
UISlider* ##name##Ptr = new UISlider(window); \
UISlider& ##name## = *##name##Ptr;