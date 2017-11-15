#pragma once
#include "UIWindow.hpp"
#include "UIAllElements.hpp"
#include "Text.hpp"
#include <string>

#include "Engine.hpp"

UIWindow* createCameraConfigWindow()
{
	auto win = new UIWindow("Camera Config", irect(0, 500, 300, 140), 4, &Engine::window);

	Text2D::TextStyle styleA(Engine::assets.getFont("clearsansb"), 16);

	C_SLIDER(sliExposure, win);
	sliExposure.setName("exposure");
	sliExposure.setDescription("Exposure");
	sliExposure.valType = UISlider::Float;
	sliExposure.setLimits(glm::fvec2(0.1f, 10.f));
	sliExposure.value.f = 2.f;
	sliExposure.init(glm::fvec2(15, 30), 270);
	sliExposure.setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UISlider* _this = (UISlider*)__this;
		Engine::cfg.render.camera.setExposure(_this->value.f);
	});

	C_SLIDER(sliFOV, win);
	sliFOV.setName("fov");
	sliFOV.setDescription("FOV");
	sliFOV.valType = UISlider::Float;
	sliFOV.setLimits(glm::fvec2(30.f, 150.f));
	sliFOV.value.f = 100.f;
	sliFOV.init(glm::fvec2(15, 80), 270);
	sliFOV.setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UISlider* _this = (UISlider*)__this;
		Engine::cfg.render.camera.setFOV(_this->value.f);
	});

	win->setMovable(true);
	win->addElement(sliExposurePtr);
	win->addElement(sliFOVPtr);

	return win;
}