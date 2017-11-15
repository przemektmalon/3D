#pragma once
#include "UIWindow.hpp"
#include "UIAllElements.hpp"
#include "Text.hpp"
#include <string>

#include "Engine.hpp"

UIWindow* createRenderConfigWindow()
{
	auto win = new UIWindow("Render Config", irect(50, 50, 300, 500), 4, &Engine::window);

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

		_this->text.setString(std::string("FPS: ") + std::to_string(1.f / Engine::dt.getSecondsf()));
	});

	C_BUTTON(butToggleWire, win);
	butToggleWire.setName("button");
	butToggleWire.setSize(glm::fvec2(258, 30));
	butToggleWire.getText().setStyle(styleA);
	butToggleWire.getText().setCharSize(18);
	butToggleWire.setString(SSTR("Toggle wireframe"));
	butToggleWire.getText().setTextOrigin(Text2D::MiddleMiddle);
	butToggleWire.setPosition(glm::fvec2(15, 70));
	butToggleWire.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.toggleDrawWireframe();
	});

	C_SLIDER(sliSSAORad, win);
	sliSSAORad.setName("ssaorad");
	sliSSAORad.setDescription("SSAO Radius");
	sliSSAORad.valType = UISlider::Float;
	sliSSAORad.setLimits(glm::fvec2(0.1f, 40.f));
	sliSSAORad.binding.f = &Engine::cfg.render.ssao.sampleRadius;
	sliSSAORad.value.f = 5.f;
	sliSSAORad.init(glm::fvec2(15, 110), 270);

	C_SLIDER(sliSSAOInt, win);
	sliSSAOInt.setName("ssaoint");
	sliSSAOInt.setDescription("SSAO Intensity");
	sliSSAOInt.valType = UISlider::Float;
	sliSSAOInt.setLimits(glm::fvec2(0.1f, 40.f));
	sliSSAOInt.binding.f = &Engine::cfg.render.ssao.intensity;
	sliSSAOInt.value.f = 2.f;
	sliSSAOInt.init(glm::fvec2(15, 160), 270);

	C_SLIDER(sliSSAOSca, win);
	sliSSAOSca.setName("ssaoscale");
	sliSSAOSca.setDescription("SSAO Frame Scale");
	sliSSAOSca.valType = UISlider::Float;
	sliSSAOSca.setLimits(glm::fvec2(100.f, 2000.f));
	sliSSAOSca.binding.f = &Engine::cfg.render.ssao.projScale;
	sliSSAOSca.value.f = 500.f;
	sliSSAOSca.init(glm::fvec2(15, 210), 270);

	C_SLIDER(sliMinLight, win);
	sliMinLight.setName("minlight");
	sliMinLight.setDescription("Min Light Constant");
	sliMinLight.valType = UISlider::Float;
	sliMinLight.setLimits(glm::fvec2(1000.f, 15000.f));
	sliMinLight.binding.f = &Engine::cfg.render.minimumLightConstant;
	sliMinLight.value.f = 10000.f;
	sliMinLight.init(glm::fvec2(15, 260), 270);

	win->setMovable(true);
	win->addElement(lblFPSPtr);
	win->addElement(sliSSAORadPtr);
	win->addElement(sliSSAOIntPtr);
	win->addElement(sliSSAOScaPtr);
	win->addElement(sliMinLightPtr);
	win->addElement(butToggleWirePtr);

	return win;
}