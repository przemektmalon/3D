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
	lblFPS.setPosition(glm::fvec2(20, 8));
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
	butToggleWire.getText().setTextOrigin(Text2D::BotLeft);
	butToggleWire.setPosition(glm::fvec2(20, 340));
	butToggleWire.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.toggleDrawWireframe();
	});

	C_SLIDER(slider, win);
	slider.setName("slider");
	slider.init(irect(20, 420, 108, 10));
	slider.valType = UISlider::Float;
	slider.setLimits(glm::fvec2(0.1f, 10.f));
	slider.binding.f = &Engine::cfg.render.ssao.sampleRadius;
	slider.value.f = 0.1f;

	win->setMovable(true);
	win->addElement(lblFPSPtr);
	win->addElement(sliderPtr);
	win->addElement(butToggleWirePtr);

	return win;
}