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
	tabEdits.setName("tab");
	tabEdits.setPosition(glm::fvec2(10, 70));
	tabEdits.setDimensions(glm::fvec2(305, 565));
	tabEdits.addTab("First");
	tabEdits.addTab("Second");
	tabEdits.addTab("Third");
	tabEdits.addTab("Fourth");

	C_BUTTON(butShootBall, win);
	butShootBall.setName("shootball");
	butShootBall.setSize(glm::fvec2(170, 30));
	butShootBall.getText().setStyle(styleA);
	butShootBall.setString(SSTR("Shoot Ball"));
	butShootBall.getText().setTextOrigin(Text2D::MiddleMiddle);
	butShootBall.setPosition(glm::fvec2(20, 93));
	butShootBall.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& me) -> void {
		UIButton* _this = (UIButton*)__this;
		auto i = Engine::world->addModelInstance("pbrsphere", Engine::world->getWorldRootNode());
		i->sgNode->transform.scale(10.f);
		i->sgNode->transform.setTranslation(Engine::cam.pos);
		auto col = new btSphereShape(10.f);
		i->makePhysicsObject(col, 10.f);
		glm::fvec3 dir = Engine::cam.getDirectionVector();
		float force = 400.f;
		i->physicsObject->rigidBody->applyImpulse(btVector3(force * dir.x, force * dir.y, force * dir.z), btVector3(0.f, 0.f, 0.f));
		if (Engine::qpc.now() % 3 == 0)
			i->overwriteMaterial(0, 0, Engine::assets.getMaterial("copper"));
		else if (Engine::qpc.now() % 3 == 1)
			i->overwriteMaterial(0, 0, Engine::assets.getMaterial("greasymetal"));

		Engine::world->sg.updateAll();
		Engine::world->updateGLBuffers();
	});

	C_BUTTON(butShootBox, win);
	butShootBox.setName("shootbox");
	butShootBox.setSize(glm::fvec2(170, 30));
	butShootBox.getText().setStyle(styleA);
	butShootBox.setString(SSTR("Shoot Box"));
	butShootBox.getText().setTextOrigin(Text2D::MiddleMiddle);
	butShootBox.setPosition(glm::fvec2(20, 93));
	butShootBox.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& me) -> void {
		UIButton* _this = (UIButton*)__this;
		auto i = Engine::world->addModelInstance("hollowbox", Engine::world->getWorldRootNode());
		i->sgNode->transform.scale(10.f);
		i->sgNode->transform.setTranslation(Engine::cam.pos);
		auto col = new btBoxShape(glm::fvec3(10.f));
		i->makePhysicsObject(col, 10.f);
		glm::fvec3 dir = Engine::cam.getDirectionVector();
		float force = 400.f;
		i->physicsObject->rigidBody->applyImpulse(btVector3(force * dir.x, force * dir.y, force * dir.z), btVector3(0.f, 0.f, 0.f));
		if (Engine::qpc.now() % 3 == 0)
			i->overwriteMaterial(0, 0, Engine::assets.getMaterial("mahogany"));
		else if (Engine::qpc.now() % 3 == 1)
			i->overwriteMaterial(0, 0, Engine::assets.getMaterial("marble"));

		Engine::world->sg.updateAll();
		Engine::world->updateGLBuffers();
	});

	tabEdits.addTabElement("First", butShootBallPtr);
	tabEdits.addTabElement("Second", butShootBoxPtr);

	win->addElement(butToggleEditPtr);
	win->addElement(butTogglePhysicsPtr);
	win->addElement(tabEditsPtr);

	return win;
}