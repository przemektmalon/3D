#pragma once
#include "UIWindow.hpp"
#include "UIAllElements.hpp"
#include "Text.hpp"
#include <string>

#include "Engine.hpp"

void createModelInfoWindow(UIWindow*& win)
{
	win = new UIWindow(irect(0, 0, 300, 500), 4, &Engine::window);
	win->setTitle("UI Window");

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

	lblFPS.setOnMouseDown([](UIWindow* win, UIElement* __this, MouseEvent& me) -> void {
		UILabel* _this = (UILabel*)__this;

		return;
	});

	C_BUTTON(butToggleWire, win);
	butToggleWire.setName("button");
	butToggleWire.setSize(glm::fvec2(250, 30));
	butToggleWire.getText().setStyle(styleA);
	butToggleWire.getText().setCharSize(18);
	butToggleWire.setString(SSTR("Toggle wireframe"));
	butToggleWire.getText().setTextOrigin(Text2D::BotLeft);
	butToggleWire.setPosition(glm::fvec2(20, 340));
	butToggleWire.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.toggleDrawWireframe();
	});

	C_BUTTON(butRes0, win);
	butRes0.setName("res0");
	butRes0.setSize(glm::fvec2(250, 30));
	butRes0.getText().setStyle(styleA);
	butRes0.getText().setCharSize(18);
	butRes0.setString(SSTR("3840x2160"));
	butRes0.getText().setTextOrigin(Text2D::BotLeft);
	butRes0.setPosition(glm::fvec2(20, 300));
	butRes0.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setResolution(0);
	});

	C_BUTTON(butRes1, win);
	butRes1.setName("res1");
	butRes1.setSize(glm::fvec2(250, 30));
	butRes1.getText().setStyle(styleA);
	butRes1.getText().setCharSize(18);
	butRes1.setString(SSTR("1920x1080"));
	butRes1.getText().setTextOrigin(Text2D::BotLeft);
	butRes1.setPosition(glm::fvec2(20, 260));
	butRes1.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setResolution(1);
	});

	C_BUTTON(butRes2, win);
	butRes2.setName("res2");
	butRes2.setSize(glm::fvec2(250, 30));
	butRes2.getText().setStyle(styleA);
	butRes2.getText().setCharSize(18);
	butRes2.setString(SSTR("1280x720"));
	butRes2.getText().setTextOrigin(Text2D::BotLeft);
	butRes2.setPosition(glm::fvec2(20, 220));
	butRes2.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setResolution(5);
	});

	C_BUTTON(butRes3, win);
	butRes3.setName("res3");
	butRes3.setSize(glm::fvec2(250, 30));
	butRes3.getText().setStyle(styleA);
	butRes3.getText().setCharSize(18);
	butRes3.setString(SSTR("960x540"));
	butRes3.getText().setTextOrigin(Text2D::BotLeft);
	butRes3.setPosition(glm::fvec2(20, 180));
	butRes3.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setResolution(7);
	});

	C_SLIDER(slider, win);
	slider.setName("slider");
	slider.init(irect(20, 420, 250, 10));
	slider.valType = UISlider::Float;
	slider.setLimits(glm::fvec2(0.f, 100.f));

	win->setMovable(true);
	win->addElement(sliderPtr);
	win->addElement(lblFPSPtr);
	win->addElement(butToggleWirePtr);
	win->addElement(butRes0Ptr);
	win->addElement(butRes1Ptr);
	win->addElement(butRes2Ptr);
	win->addElement(butRes3Ptr);

}