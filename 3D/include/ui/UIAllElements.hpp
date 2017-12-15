#pragma once
#include "UIButton.hpp"
#include "UILabel.hpp"
#include "UISlider.hpp"
#include "UISwitchButton.hpp"
#include "UIInputField.hpp"
#include "UIMultiTab.hpp"

#define C_LABEL(name,window) \
UILabel* ##name##Ptr = new UILabel(window); \
UILabel& ##name## = *##name##Ptr;

#define C_BUTTON(name,window) \
UIButton* ##name##Ptr = new UIButton(window); \
UIButton& ##name## = *##name##Ptr;

#define C_SLIDER(name,window) \
UISlider* ##name##Ptr = new UISlider(window); \
UISlider& ##name## = *##name##Ptr;

#define C_INPUT(name,window) \
UIInputField* ##name##Ptr = new UIInputField(window); \
UIInputField& ##name## = *##name##Ptr;

#define C_MULTITAB(name,window) \
UIMultiTab* ##name##Ptr = new UIMultiTab(window); \
UIMultiTab& ##name = *##name##Ptr;