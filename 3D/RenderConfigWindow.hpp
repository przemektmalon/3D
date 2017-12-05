#pragma once
#include "UIWindow.hpp"
#include "UIAllElements.hpp"
#include "Text.hpp"
#include <string>

#include "Engine.hpp"

UIWindow* createRenderConfigWindow()
{
	auto win = new UIWindow("Render Config", irect(0, 0, 300, 570), 4, &Engine::window);

	Text2D::TextStyle styleA(Engine::assets.getFont("clearsansb"), 16);

	C_LABEL(lblFPS, win);
	lblFPS.setName("fps");
	lblFPS.setStyle(styleA);
	lblFPS.setString(String<32>("FPS: "));
	lblFPS.setTextOrigin(Text2D::TopLeft);
	lblFPS.setPosition(glm::fvec2(15, 30));
	lblFPS.setUpdate(
		[](UIWindow * win, UIElement* __this) -> void {
		UILabel* _this = (UILabel*)__this;

		_this->text.setString(std::string("FPS: ") + std::to_string(1.f / Engine::deltaTime.getSecondsf()));
	});

	C_SLIDER(sliSSAORad, win);
	sliSSAORad.setName("ssaorad");
	sliSSAORad.setDescription("SSAO Radius");
	sliSSAORad.valType = UISlider::Float;
	sliSSAORad.setLimits(glm::fvec2(0.1f, 40.f));
	sliSSAORad.value.f = 15.f;
	sliSSAORad.init(glm::fvec2(15, 70), 270);
	sliSSAORad.setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UISlider* _this = (UISlider*)__this;
		Engine::cfg.render.ssao.setSampleRadius(_this->value.f);
	});

	C_SLIDER(sliSSAOInt, win);
	sliSSAOInt.setName("ssaoint");
	sliSSAOInt.setDescription("SSAO Intensity");
	sliSSAOInt.valType = UISlider::Float;
	sliSSAOInt.setLimits(glm::fvec2(0.1f, 40.f));
	sliSSAOInt.value.f = 4.f;
	sliSSAOInt.init(glm::fvec2(15, 110), 270);
	sliSSAOInt.setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UISlider* _this = (UISlider*)__this;
		Engine::cfg.render.ssao.setIntensity(_this->value.f);
	});

	C_SLIDER(sliSSAOSca, win);
	sliSSAOSca.setName("ssaoscale");
	sliSSAOSca.setDescription("SSAO Frame Scale");
	sliSSAOSca.valType = UISlider::Float;
	sliSSAOSca.setLimits(glm::fvec2(100.f, 2000.f));
	sliSSAOSca.value.f = 500.f;
	sliSSAOSca.init(glm::fvec2(15, 160), 270);
	sliSSAOSca.setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UISlider* _this = (UISlider*)__this;
		Engine::cfg.render.ssao.setProjScale(_this->value.f);
	});

	C_SLIDER(sliSSAOSam, win);
	sliSSAOSam.setName("ssaosamples");
	sliSSAOSam.setDescription("SSAO Samples");
	sliSSAOSam.valType = UISlider::Integer;
	sliSSAOSam.setLimits(glm::ivec2(5, 200));
	sliSSAOSam.value.i = 50;
	sliSSAOSam.init(glm::fvec2(15, 210), 270);
	sliSSAOSam.setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UISlider* _this = (UISlider*)__this;
		Engine::cfg.render.ssao.setSamples(_this->value.i);
	});

	C_SLIDER(sliSSAOSpi, win);
	sliSSAOSpi.setName("ssaospirals");
	sliSSAOSpi.setDescription("SSAO Spirals");
	sliSSAOSpi.valType = UISlider::Integer;
	sliSSAOSpi.setLimits(glm::ivec2(0, 200));
	sliSSAOSpi.value.i = 9;
	sliSSAOSpi.init(glm::fvec2(15, 260), 270);
	sliSSAOSpi.setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UISlider* _this = (UISlider*)__this;
		Engine::cfg.render.ssao.setSpiralTurns(_this->value.i);
	});

	C_SLIDER(sliMinLight, win);
	sliMinLight.setName("minlight");
	sliMinLight.setDescription("Min Light Constant");
	sliMinLight.valType = UISlider::Float;
	sliMinLight.setLimits(glm::fvec2(1000.f, 15000.f));
	sliMinLight.binding.f = &Engine::cfg.render.minimumLightConstant; // No need for update function here, just bind the value to the the appropriate variable
	sliMinLight.value.f = 10000.f;
	sliMinLight.init(glm::fvec2(15, 310), 270);

	C_BUTTON(butToggleWire, win);
	butToggleWire.setName("button");
	butToggleWire.setSize(glm::fvec2(270, 30));
	butToggleWire.getText().setStyle(styleA);
	butToggleWire.getText().setCharSize(18);
	butToggleWire.setString(SSTR("Toggle wireframe"));
	butToggleWire.getText().setTextOrigin(Text2D::MiddleMiddle);
	butToggleWire.setPosition(glm::fvec2(15, 370));
	butToggleWire.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.toggleDrawWireframe();
	});

	C_BUTTON(butToggleVSync, win);
	butToggleVSync.setName("vsync");
	butToggleVSync.setSize(glm::fvec2(270, 30));
	butToggleVSync.getText().setStyle(styleA);
	butToggleVSync.getText().setCharSize(18);
	butToggleVSync.setString(SSTR("Toggle vertical sync"));
	butToggleVSync.getText().setTextOrigin(Text2D::MiddleMiddle);
	butToggleVSync.setPosition(glm::fvec2(15, 420));
	butToggleVSync.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.toggleVSync();
	});

	C_BUTTON(butScreenshot, win);
	butScreenshot.setName("screenshot");
	butScreenshot.setSize(glm::fvec2(270, 30));
	butScreenshot.getText().setStyle(styleA);
	butScreenshot.getText().setCharSize(18);
	butScreenshot.setString(SSTR("Screenshot"));
	butScreenshot.getText().setTextOrigin(Text2D::MiddleMiddle);
	butScreenshot.setPosition(glm::fvec2(15, 470));
	butScreenshot.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.screenshot();
	});

	C_BUTTON(butReloadShaders, win);
	butReloadShaders.setName("shaders");
	butReloadShaders.setSize(glm::fvec2(270, 30));
	butReloadShaders.getText().setStyle(styleA);
	butReloadShaders.getText().setCharSize(18);
	butReloadShaders.setString(SSTR("Reload shaders"));
	butReloadShaders.getText().setTextOrigin(Text2D::MiddleMiddle);
	butReloadShaders.setPosition(glm::fvec2(15, 520));
	butReloadShaders.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.reloadAllShaders();
	});

	win->setMovable(true);
	win->addElement(lblFPSPtr);
	win->addElement(sliSSAORadPtr);
	win->addElement(sliSSAOIntPtr);
	win->addElement(sliSSAOScaPtr);
	win->addElement(sliSSAOSamPtr);
	win->addElement(sliSSAOSpiPtr);
	win->addElement(sliMinLightPtr);
	win->addElement(butScreenshotPtr);
	win->addElement(butReloadShadersPtr);
	win->addElement(butToggleVSyncPtr);
	win->addElement(butToggleWirePtr);

	return win;
}