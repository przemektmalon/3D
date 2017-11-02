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

	C_BUTTON(button, win);
	button.setName("button");
	button.setSize(glm::fvec2(250, 30));
	button.getText().setStyle(styleA);
	button.getText().setCharSize(18);
	button.setString(SSTR("Button!"));
	button.getText().setTextOrigin(Text2D::BotLeft);
	button.setPosition(glm::fvec2(20, 340));

	button.setOnMouseDown([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		_this->setString(SSTR("Button Pressed!"));
	});

	button.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		if (_this->isHovered())
			_this->setString(SSTR("Button Hovered!"));
		else
			_this->setString(SSTR("Button!"));
	});

	button.setOnMouseEnter([](UIWindow* win, UIElement* __this, MouseEvent& me) -> void {
		UIButton* _this = (UIButton*)__this;
		_this->setString(SSTR("Button Hovered!"));
	});

	button.setOnMouseLeave([](UIWindow* win, UIElement* __this, MouseEvent& me) -> void {
		UIButton* _this = (UIButton*)__this;
		_this->setString(SSTR("Button!"));
	});

	C_SLIDER(slider, win);
	slider.setName("slider");
	slider.init(irect(20, 420, 250, 10));
	slider.valType = UISlider::Float;
	slider.setLimits(glm::fvec2(0.f, 100.f));

	win->setMovable(true);
	win->addElement(sliderPtr);
	win->addElement(lblFPSPtr);
	win->addElement(buttonPtr);
}