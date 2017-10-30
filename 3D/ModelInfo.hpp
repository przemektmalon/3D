#pragma once
#include "UIWindow.hpp"
#include "UIAllElements.hpp"
#include "Text.hpp"
#include <string>

#include "Engine.hpp"

void createModelInfoWindow(UIWindow*& win)
{
	win = new UIWindow(UIRect(50, 50, 140, 50), 4, &Engine::window);
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

	/*lblFPS.setOnMouseDown([](UIWindow* win, UIElement* __this, MouseEvent& me) -> bool {
		UILabel* _this = (UILabel*)__this;

		if (Engine::window.isMouseInClientArea() && (me.getCode().code == MouseCode::M_LEFT))
		{
			if (_this->getParentWindow()->getWindowRect().contains(me.getPosition()))
			{
				return false;
			}
			Engine::select(me.getPosition());
			if (Engine::selectedID != -1)
			{
				auto i = Engine::world->getModelInstance(Engine::selectedID);
				if (!i)
					return false;
				String<64> line("Model name: ");
				line.append(i->model->getName());
				_this->setString(line);
			}
			return true;
		}
	});
	
	C_LABEL(lblModelInfo, win);
	lblModelInfo.setStyle(styleA);
	lblModelInfo.setString(String<32>("Position: "));
	lblModelInfo.setPosition(glm::fvec2(4, 25));

	lblModelInfo.setOnMouseDown([](UIWindow* win, UIElement* __this, MouseEvent& me) -> bool {
		UILabel* _this = (UILabel*)__this;

		if (Engine::selectedID != -1 && Engine::selectedID != 0)
		{
			auto i = Engine::world->getModelInstance(Engine::selectedID);
			if (!i)
				return false;

			String<256> line("Position: ");

			auto p = i->sgNode->fullTransform.getTranslation();
			auto r = i->sgNode->fullTransform.getQuat();
			auto s = i->sgNode->fullTransform.getScale();

			std::string pos = ("(" + std::to_string(p.x) + ", " + std::to_string(p.y) + ", " + std::to_string(p.z) + ")\n");

			pos += "Rotation: (" + std::to_string(r.x) + ", " + std::to_string(r.y) + ", " + std::to_string(r.z) + ", " + std::to_string(r.w) + ")\n";
			pos += "Scale:    (" + std::to_string(s.x) + ", " + std::to_string(s.y) + ", " + std::to_string(s.z) + ")\n";

			String<256> line2; line2.setToChars(pos.c_str());

			line.append(line2);
			_this->setString(line);
		}
		return true;
	});


	C_BUTTON(butMovXP, win);
	butMovXP.setSize(glm::fvec2(200, 200));
	butMovXP.getText().setStyle(styleA);
	butMovXP.setString(SSTR("MoveXP"));
	butMovXP.setPosition(glm::fvec2(100, 100));

	butMovXP.setOnMouseDown([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> bool {
		UIButton* _this = (UIButton*)__this;

		auto rect = _this->getBounds();
		auto mp = glm::ivec2(ke.getUIWindowPosition(_this->getParentWindow()).x, _this->getParentWindow()->getWindowRect().height - ke.getUIWindowPosition(_this->getParentWindow()).y);

		if (!rect.contains(mp))
		{
			return false;
		}
		if (Engine::selectedID != -1)
		{
			auto i = Engine::world->getModelInstance(Engine::selectedID);
			if (!i)
				return false;

			i->sgNode->transform.translate(glm::fvec3(1.f, 0.f, 0.f));
			i->sgNode->updateAll();
			Engine::world->updateGLBuffers();
		}
		return true;
	});

	butMovXP.setOnHover([](UIWindow* win, UIElement* __this, MouseEvent& me) -> bool {
		UIButton* _this = (UIButton*)__this;

		auto rect = _this->getBounds();
		auto mp = glm::ivec2(me.getUIWindowPosition(_this->getParentWindow()).x, _this->getParentWindow()->getWindowRect().height - me.getUIWindowPosition(_this->getParentWindow()).y);

		if (!rect.contains(mp))
		{
			_this->setTextColour(glm::fvec3(1.f, 1.f, 1.f));
			_this->setRectColour(glm::fvec4(0.7f, 0.2f, 0.2f, 0.8f));
			return false;
		}
		else
		{
			_this->getText().setColour(glm::fvec3(1.f, 0.f, 1.f));
			_this->setRectColour(glm::fvec4(0.7f, 0.2f, 0.2f, 1.f));

			return true;
		}
	});*/

	win->setMovable(true);
	win->addElement(lblFPSPtr);
	//win->addElement(lblModelInfoPtr);
	//win->addElement(butMovXPPtr);
}