#pragma once
#include "UIWindow.hpp"
#include "UIAllElements.hpp"
#include "Text.hpp"
#include <string>

#include "Engine.hpp"

void createModelInfoWindow(UIWindow*& win)
{
	win = new UIWindow(UIRect(50, 50, 500, 500), 4, &Engine::window);
	win->setTitle("STATS");

	Text2D::TextStyle styleA(Engine::assets.getFont("clearsansb"), 14);

	C_LABEL(lblFPS, win);
	lblFPS.setStyle(styleA);
	lblFPS.setString(String<32>("FPS: "));
	lblFPS.setTextOrigin(Text2D::TopLeft);
	lblFPS.setPosition(glm::fvec2(8, 4));
	
	lblFPS.setUpdate(
		[](UIWindow * win, UIElement* __this) -> void {
		UILabel* _this = (UILabel*)__this;

		_this->text.setString(std::string("FPS: ") + std::to_string(1.f / Engine::dt.getSecondsf()));
	});

	lblFPS.setOnMouseDown([](UIWindow* win, UIElement* __this, MouseEvent& me) -> void {
		UILabel* _this = (UILabel*)__this;

		
		return;
	});

	C_BUTTON(butMovXP, win);
	butMovXP.setSize(glm::fvec2(200, 200));
	butMovXP.getText().setStyle(styleA);
	butMovXP.setString(SSTR("MoveXP"));
	butMovXP.setPosition(glm::fvec2(100, 100));

	butMovXP.setOnMouseDown([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;

	});

	butMovXP.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;

	});

	butMovXP.setOnMouseEnter([](UIWindow* win, UIElement* __this, MouseEvent& me) -> void {
		UIButton* _this = (UIButton*)__this;

	});

	butMovXP.setOnMouseLeave([](UIWindow* win, UIElement* __this, MouseEvent& me) -> void {
		UIButton* _this = (UIButton*)__this;

		
	});

	win->setMovable(false);
	win->addElement(lblFPSPtr);
	win->addElement(butMovXPPtr);
}