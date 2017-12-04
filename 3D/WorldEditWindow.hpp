#pragma once
#include "UIWindow.hpp"
#include "UIAllElements.hpp"
#include "Text.hpp"
#include <string>
#include "Engine.hpp"

UIWindow* createWorldEditWindow()
{
	auto win = new UIWindow("World Edit", irect(300, 300, 325, 600), 4, &Engine::window);

	Text2D::TextStyle styleA(Engine::assets.getFont("clearsansb"), 16);

	C_BUTTON(butTogglePhysics, win);
	butTogglePhysics.setName("togglephysics");
	butTogglePhysics.setSize(glm::fvec2(140, 30));
	butTogglePhysics.getText().setStyle(styleA);
	butTogglePhysics.getText().setCharSize(18);
	butTogglePhysics.setString(SSTR("Toggle physics"));
	butTogglePhysics.getText().setTextOrigin(Text2D::MiddleMiddle);
	butTogglePhysics.setPosition(glm::fvec2(15, 30));
	butTogglePhysics.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.world.togglePhysics();
	});

	C_BUTTON(butToggleEdit, win);
	butToggleEdit.setName("toggleedit");
	butToggleEdit.setSize(glm::fvec2(140, 30));
	butToggleEdit.getText().setStyle(styleA);
	butToggleEdit.getText().setCharSize(18);
	butToggleEdit.setString(SSTR("Toggle editing"));
	butToggleEdit.getText().setTextOrigin(Text2D::MiddleMiddle);
	butToggleEdit.setPosition(glm::fvec2(170, 30));
	butToggleEdit.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		//Engine::cfg.world.toggleEdit();
	});

	C_MULTITAB(tabEdits, win);
	tabEdits.setPosition(glm::fvec2(10, 70));
	tabEdits.setDimensions(glm::fvec2(305, 565));
	tabEdits.addTab("First");
	tabEdits.addTab("Second");
	tabEdits.addTab("Third");
	tabEdits.addTab("Fourth");

	win->addElement(butToggleEditPtr);
	win->addElement(butTogglePhysicsPtr);
	win->addElement(tabEditsPtr);

	return win;
}