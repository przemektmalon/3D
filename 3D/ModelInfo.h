#pragma once
#include "UIWindow.h"
#include "UIAllElements.h"
#include "Text.h"

#include "Engine.h"

void createModelInfoWindow(UIWindow*& win)
{
	win = new UIWindow(UIRect(20, 50, 500, 600), 6, &Engine::window);
	win->setTitle("MODEL MANIPULATION");
	
	Text2D::TextStyle styleA(Engine::assets.getFont("clearsansb"), 14);

	C_LABEL(lblModelName, win);
	lblModelName.setStyle(styleA);
	lblModelName.setString(String<32>("Model name: "));
	lblModelName.setPosition(glm::fvec2(5, 7));

	auto mimd = [](UIElement* __this, MouseEvent& me) -> void {
		UILabel* _this = (UILabel*)__this;

		if (Engine::window.isMouseInClientArea() && (me.getCode().code == MouseCode::M_LEFT))
		{
			if (_this->getParentWindow()->getWindowRect().contains(me.getPosition()))
			{
				return;
			}
			Engine::select(me.getPosition());
			if (Engine::selectedID != -1)
			{
				auto i = Engine::world->getMeshInstance(Engine::selectedID);
				String<64> line("Model name: ");
				line.append(i->mesh->getName());
				_this->setString(line);
			}
		}
	};

	lblModelName.setOnMouseDown(mimd);
	
	C_LABEL(lblModelInfo, win);
	lblModelInfo.setStyle(styleA);
	lblModelInfo.setString(String<32>("Position: "));
	lblModelInfo.setPosition(glm::fvec2(5, 27));

	auto mimd2 = [](UIElement* __this, MouseEvent& me) -> void {
		UILabel* _this = (UILabel*)__this;

		if (Engine::selectedID != -1 && Engine::selectedID != 0)
		{
			auto i = Engine::world->getMeshInstance(Engine::selectedID);
			String<256> line("Position: ");

			auto p = i->sgNode->fullTransform.getTranslation();
			auto r = i->sgNode->fullTransform.getQRotation();
			auto s = i->sgNode->fullTransform.getScale();

			std::string pos = ("(" + std::to_string(p.x) + ", " + std::to_string(p.y) + ", " + std::to_string(p.z) + ")\n");

			pos += "Rotation: (" + std::to_string(r.x) + ", " + std::to_string(r.y) + ", " + std::to_string(r.z) + ", " + std::to_string(r.w) + ")\n";
			pos += "Scale:    (" + std::to_string(s.x) + ", " + std::to_string(s.y) + ", " + std::to_string(s.z) + ")\n";

			String<256> line2; line2.setToChars(pos.c_str());

			line.append(line2);
			_this->setString(line);
		}
	};

	lblModelInfo.setOnMouseDown(mimd2);


	C_BUTTON(butMovXP, win);
	butMovXP.rect.setBounds(irect(200, 300, 50, 50));
	butMovXP.text.setStyle(styleA);
	butMovXP.text.setString(String<32>("MoveXP"));

	auto bmxp = [](UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;

		auto rect = _this->text.getBoundingBox();
		auto mp = glm::ivec2(ke.getUIWindowPosition(_this->getParentWindow()).x, _this->getParentWindow()->getWindowRect().height - ke.getUIWindowPosition(_this->getParentWindow()).y);

		if (!rect.contains(mp))
		{
			return;
		}
		if (Engine::selectedID != -1)
		{
			auto i = Engine::world->getMeshInstance(Engine::selectedID);

			i->sgNode->transform.translate(glm::fvec3(1.f, 0.f, 0.f));
			i->sgNode->updateAll();
			Engine::world->updateGLBuffers();
		}
	};

	butMovXP.setOnMouseDown(bmxp);

	win->addElement(lblModelNamePtr);
	win->addElement(lblModelInfoPtr);
	win->addElement(butMovXPPtr);
}