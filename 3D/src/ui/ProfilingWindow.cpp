#include "WindowScripts.hpp"
#include "UIWindow.hpp"
#include "UIAllElements.hpp"

void profilingWindowScripts(UIWindow* win)
{
	win->getElement("gpu")->setUpdate(
		[](UIWindow * win, UIElement* __this) -> void {
		UILabel* _this = (UILabel*)__this;

		_this->text.setString(std::string("Light update: ") + std::to_string((double)Engine::profiler.getTime("gpuBuffer").getMilliSeconds()) + " ms");
	});

	win->getElement("gbuffer")->setUpdate(
		[](UIWindow * win, UIElement* __this) -> void {
		UILabel* _this = (UILabel*)__this;

		_this->text.setString(std::string("Gbuffer pass: ") + std::to_string((double)Engine::profiler.getTime("gBuffer").getMilliSeconds()) + " ms");
	});

	win->getElement("shadow")->setUpdate(
		[](UIWindow * win, UIElement* __this) -> void {
		UILabel* _this = (UILabel*)__this;

		_this->text.setString(std::string("Shadow pass: ") + std::to_string((double)Engine::profiler.getTime("shadow").getMilliSeconds()) + " ms");
	});

	win->getElement("ssao")->setUpdate(
		[](UIWindow * win, UIElement* __this) -> void {
		UILabel* _this = (UILabel*)__this;

		_this->text.setString(std::string("SSAO pass: ") + std::to_string((double)Engine::profiler.getTime("ssao").getMilliSeconds()) + " ms");
	});

	win->getElement("shading")->setUpdate(
		[](UIWindow * win, UIElement* __this) -> void {
		UILabel* _this = (UILabel*)__this;

		_this->text.setString(std::string("Shading pass: ") + std::to_string((double)Engine::profiler.getTime("light").getMilliSeconds()) + " ms");
	});

	win->getElement("screen")->setUpdate(
		[](UIWindow * win, UIElement* __this) -> void {
		UILabel* _this = (UILabel*)__this;

		_this->text.setString(std::string("Screen pass: ") + std::to_string((double)Engine::profiler.getTime("screen").getMilliSeconds()) + " ms");
	});

	win->getElement("physics")->setUpdate(
		[](UIWindow * win, UIElement* __this) -> void {
		UILabel* _this = (UILabel*)__this;

		_this->text.setString(std::string("Physics: ") + std::to_string((double)Engine::profiler.getTime("physics").getMilliSeconds()) + " ms");
	});

	win->getElement("frame")->setUpdate(
		[](UIWindow * win, UIElement* __this) -> void {
		UILabel* _this = (UILabel*)__this;

		_this->text.setString(std::string("Frame time: ") + std::to_string((double)Engine::profiler.getTime("frame").getMilliSeconds()) + " ms");
	});
}