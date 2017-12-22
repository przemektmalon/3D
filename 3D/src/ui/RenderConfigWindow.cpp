#include "WindowScripts.hpp"
#include "UIWindow.hpp"
#include "UIAllElements.hpp"
#include "Text.hpp"
#include <string>
#include "Engine.hpp"

void renderConfigWindowScripts(UIWindow * win)
{
	win->getElement("fps")->setUpdate([](UIWindow * win, UIElement* __this) -> void {
		UILabel* _this = (UILabel*)__this;
		_this->text.setString(std::string("FPS: ") + std::to_string(1.f / Engine::deltaTime.getSecondsf()));
	});

	win->getElement("ssaorad")->setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UISlider* _this = (UISlider*)__this;
		Engine::cfg.render.ssao.setSampleRadius(_this->value.f);
	});

	win->getElement("ssaoint")->setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UISlider* _this = (UISlider*)__this;
		Engine::cfg.render.ssao.setIntensity(_this->value.f);
	});

	win->getElement("ssaosamples")->setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UISlider* _this = (UISlider*)__this;
		Engine::cfg.render.ssao.setSamples(_this->value.i);
	});

	win->getElement("ssaospirals")->setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UISlider* _this = (UISlider*)__this;
		Engine::cfg.render.ssao.setSpiralTurns(_this->value.i);
	});

	win->getElement("togglewire")->setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		Engine::cfg.render.toggleDrawWireframe();
	});

	win->getElement("toggleaabb")->setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		Engine::cfg.render.toggleDrawAABB();
	});

	win->getElement("togglevsync")->setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		Engine::cfg.render.toggleVSync();
	});

	win->getElement("reloadshaders")->setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		Engine::cfg.render.reloadAllShaders();
	});

	win->getElement("screenshot")->setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		Engine::cfg.render.screenshot();
	});

	win->getElement("exposure")->setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UISlider* _this = (UISlider*)__this;
		Engine::cfg.render.camera.setExposure(_this->value.f);
	});

	win->getElement("fov")->setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UISlider* _this = (UISlider*)__this;
		Engine::cfg.render.camera.setFOV(_this->value.f);
	});

	win->getElement("shaded")->setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		Engine::cfg.render.setRenderMode(0);
	});

	win->getElement("albedo")->setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		Engine::cfg.render.setRenderMode(1);
	});

	win->getElement("normal")->setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		Engine::cfg.render.setRenderMode(2);
	});

	win->getElement("ssao")->setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		Engine::cfg.render.setRenderMode(3);
	});

	win->getElement("depth")->setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		Engine::cfg.render.setRenderMode(4);
	});

	win->getElement("res0")->setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		Engine::cfg.render.setResolution(0);
	});

	win->getElement("res1")->setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		Engine::cfg.render.setResolution(1);
	});

	win->getElement("res2")->setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		Engine::cfg.render.setResolution(2);
	});

	win->getElement("res3")->setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		Engine::cfg.render.setResolution(3);
	});

	win->getElement("res5")->setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		Engine::cfg.render.setResolution(5);
	});

	win->getElement("res6")->setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		Engine::cfg.render.setResolution(6);
	});

	win->getElement("res7")->setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		Engine::cfg.render.setResolution(7);
	});

	win->getElement("res8")->setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		Engine::cfg.render.setResolution(8);
	});
}
