#pragma once
#include "UIWindow.hpp"
#include "UIAllElements.hpp"
#include "Text.hpp"
#include <string>

#include "Engine.hpp"

UIWindow* createRenderModeWindow()
{
	auto win = new UIWindow("Render Modes", irect(300, 355, 100, 230), 4, &Engine::window);

	Text2D::TextStyle styleA(Engine::assets.getFont("clearsansb"), 16);

	C_BUTTON(butShaded, win);
	butShaded.setName("shaded");
	butShaded.setSize(glm::fvec2(70, 30));
	butShaded.getText().setStyle(styleA);
	butShaded.getText().setCharSize(18);
	butShaded.setString(SSTR("Shaded"));
	butShaded.getText().setTextOrigin(Text2D::MiddleMiddle);
	butShaded.setPosition(glm::fvec2(15, 30));
	butShaded.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setRenderMode(0);
	});

	C_BUTTON(butAlbedo, win);
	butAlbedo.setName("albedo");
	butAlbedo.setSize(glm::fvec2(70, 30));
	butAlbedo.getText().setStyle(styleA);
	butAlbedo.getText().setCharSize(18);
	butAlbedo.setString(SSTR("Albedo"));
	butAlbedo.getText().setTextOrigin(Text2D::MiddleMiddle);
	butAlbedo.setPosition(glm::fvec2(15, 70));
	butAlbedo.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setRenderMode(1);
	});

	C_BUTTON(butNormal, win);
	butNormal.setName("normal");
	butNormal.setSize(glm::fvec2(70, 30));
	butNormal.getText().setStyle(styleA);
	butNormal.getText().setCharSize(18);
	butNormal.setString(SSTR("Normal"));
	butNormal.getText().setTextOrigin(Text2D::MiddleMiddle);
	butNormal.setPosition(glm::fvec2(15, 110));
	butNormal.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setRenderMode(2);
	});

	C_BUTTON(butSSAO, win);
	butSSAO.setName("ssao");
	butSSAO.setSize(glm::fvec2(70, 30));
	butSSAO.getText().setStyle(styleA);
	butSSAO.getText().setCharSize(18);
	butSSAO.setString(SSTR("SSAO"));
	butSSAO.getText().setTextOrigin(Text2D::MiddleMiddle);
	butSSAO.setPosition(glm::fvec2(15, 150));
	butSSAO.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setRenderMode(3);
	});

	C_BUTTON(butDepth, win);
	butDepth.setName("depth");
	butDepth.setSize(glm::fvec2(70, 30));
	butDepth.getText().setStyle(styleA);
	butDepth.getText().setCharSize(18);
	butDepth.setString(SSTR("Depth"));
	butDepth.getText().setTextOrigin(Text2D::MiddleMiddle);
	butDepth.setPosition(glm::fvec2(15, 190));
	butDepth.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setRenderMode(4);
	});

	win->setMovable(true);
	win->addElement(butShadedPtr);
	win->addElement(butAlbedoPtr);
	win->addElement(butNormalPtr);
	win->addElement(butSSAOPtr);
	win->addElement(butDepthPtr);

	return win;
}