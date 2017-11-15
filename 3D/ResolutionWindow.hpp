#pragma once
#include "UIWindow.hpp"
#include "UIAllElements.hpp"
#include "Text.hpp"
#include <string>

#include "Engine.hpp"

UIWindow* createResolutionWindow()
{
	auto win = new UIWindow("Resolution", irect(300, 0, 160, 355), 4, &Engine::window);

	Text2D::TextStyle styleA(Engine::assets.getFont("clearsansb"), 16);

	C_BUTTON(butRes0, win);
	butRes0.setName("res0");
	butRes0.setSize(glm::fvec2(130, 30));
	butRes0.getText().setStyle(styleA);
	butRes0.getText().setCharSize(18);
	butRes0.setString(SSTR("3840x2160"));
	butRes0.getText().setTextOrigin(Text2D::MiddleMiddle);
	butRes0.setPosition(glm::fvec2(15, 30));
	butRes0.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setResolution(0);
	});

	C_BUTTON(butRes1, win);
	butRes1.setName("res1");
	butRes1.setSize(glm::fvec2(130, 30));
	butRes1.getText().setStyle(styleA);
	butRes1.getText().setCharSize(18);
	butRes1.setString(SSTR("1920x1080"));
	butRes1.getText().setTextOrigin(Text2D::MiddleMiddle);
	butRes1.setPosition(glm::fvec2(15, 70));
	butRes1.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setResolution(1);
	});

	C_BUTTON(butRes2, win);
	butRes2.setName("res2");
	butRes2.setSize(glm::fvec2(130, 30));
	butRes2.getText().setStyle(styleA);
	butRes2.getText().setCharSize(18);
	butRes2.setString(SSTR("1600x900"));
	butRes2.getText().setTextOrigin(Text2D::MiddleMiddle);
	butRes2.setPosition(glm::fvec2(15, 110));
	butRes2.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setResolution(2);
	});

	C_BUTTON(butRes3, win);
	butRes3.setName("res3");
	butRes3.setSize(glm::fvec2(130, 30));
	butRes3.getText().setStyle(styleA);
	butRes3.getText().setCharSize(18);
	butRes3.setString(SSTR("1546x864"));
	butRes3.getText().setTextOrigin(Text2D::MiddleMiddle);
	butRes3.setPosition(glm::fvec2(15, 150));
	butRes3.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setResolution(3);
	});

	/*C_BUTTON(butRes4, win);
	butRes4.setName("res4");
	butRes4.setSize(glm::fvec2(130, 30));
	butRes4.getText().setStyle(styleA);
	butRes4.getText().setCharSize(18);
	butRes4.setString(SSTR("1336x768"));
	butRes4.getText().setTextOrigin(Text2D::MiddleMiddle);
	butRes4.setPosition(glm::fvec2(15, 190));
	butRes4.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setResolution(4);
	});*/

	C_BUTTON(butRes5, win);
	butRes5.setName("res5");
	butRes5.setSize(glm::fvec2(130, 30));
	butRes5.getText().setStyle(styleA);
	butRes5.getText().setCharSize(18);
	butRes5.setString(SSTR("1280x720"));
	butRes5.getText().setTextOrigin(Text2D::MiddleMiddle);
	butRes5.setPosition(glm::fvec2(15, 190));
	butRes5.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setResolution(5);
	});

	C_BUTTON(butRes6, win);
	butRes6.setName("res6");
	butRes6.setSize(glm::fvec2(130, 30));
	butRes6.getText().setStyle(styleA);
	butRes6.getText().setCharSize(18);
	butRes6.setString(SSTR("1024x576"));
	butRes6.getText().setTextOrigin(Text2D::MiddleMiddle);
	butRes6.setPosition(glm::fvec2(15, 230));
	butRes6.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setResolution(6);
	});

	C_BUTTON(butRes7, win);
	butRes7.setName("res7");
	butRes7.setSize(glm::fvec2(130, 30));
	butRes7.getText().setStyle(styleA);
	butRes7.getText().setCharSize(18);
	butRes7.setString(SSTR("960x540"));
	butRes7.getText().setTextOrigin(Text2D::MiddleMiddle);
	butRes7.setPosition(glm::fvec2(15, 270));
	butRes7.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setResolution(7);
	});

	C_BUTTON(butRes8, win);
	butRes8.setName("res8");
	butRes8.setSize(glm::fvec2(130, 30));
	butRes8.getText().setStyle(styleA);
	butRes8.getText().setCharSize(18);
	butRes8.setString(SSTR("960x540"));
	butRes8.getText().setTextOrigin(Text2D::MiddleMiddle);
	butRes8.setPosition(glm::fvec2(15, 310));
	butRes8.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setResolution(8);
	});

	win->setMovable(true);
	win->addElement(butRes0Ptr);
	win->addElement(butRes1Ptr);
	win->addElement(butRes2Ptr);
	win->addElement(butRes3Ptr);
	win->addElement(butRes5Ptr);
	win->addElement(butRes6Ptr);
	win->addElement(butRes7Ptr);
	win->addElement(butRes8Ptr);

	return win;
}