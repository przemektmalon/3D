#pragma once
#include "UIWindow.hpp"
#include "UIAllElements.hpp"
#include "Text.hpp"
#include <string>

#include "Engine.hpp"

UIWindow* createProfilingWindow()
{
	auto win = new UIWindow("Profiling", irect(1920-246, 0, 246, 345), 4, &Engine::window);

	Text2D::TextStyle styleA(Engine::assets.getFont("clearsansb"), 16);

	C_LABEL(lblGPUUpdate, win);
	lblGPUUpdate.setName("lightupdate");
	lblGPUUpdate.setStyle(styleA);
	lblGPUUpdate.setString(String<32>("GPU buffer update: "));
	lblGPUUpdate.setTextOrigin(Text2D::TopLeft);
	lblGPUUpdate.setPosition(glm::fvec2(15, 30));
	lblGPUUpdate.setUpdate(
		[](UIWindow * win, UIElement* __this) -> void {
		UILabel* _this = (UILabel*)__this;

		_this->text.setString(std::string("Light update: ") + std::to_string((double)Engine::r->gpuBufferTime / 1000.f) + " ms");
	});

	C_LABEL(lblGBuffer, win);
	lblGBuffer.setName("gbuffer");
	lblGBuffer.setStyle(styleA);
	lblGBuffer.setString(String<32>("GBuffer pass: "));
	lblGBuffer.setTextOrigin(Text2D::TopLeft);
	lblGBuffer.setPosition(glm::fvec2(15, 70));
	lblGBuffer.setUpdate(
		[](UIWindow * win, UIElement* __this) -> void {
		UILabel* _this = (UILabel*)__this;

		_this->text.setString(std::string("Gbuffer pass: ") + std::to_string((double)Engine::r->gBufferTime / 1000.f) + " ms");
	});

	C_LABEL(lblShadow, win);
	lblShadow.setName("shadow");
	lblShadow.setStyle(styleA);
	lblShadow.setString(String<32>("Shadow pass: "));
	lblShadow.setTextOrigin(Text2D::TopLeft);
	lblShadow.setPosition(glm::fvec2(15, 110));
	lblShadow.setUpdate(
		[](UIWindow * win, UIElement* __this) -> void {
		UILabel* _this = (UILabel*)__this;

		_this->text.setString(std::string("Shadow pass: ") + std::to_string((double)Engine::r->shadowTime / 1000.f) + " ms");
	});

	C_LABEL(lblSSAO, win);
	lblSSAO.setName("ssao");
	lblSSAO.setStyle(styleA);
	lblSSAO.setString(String<32>("SSAO pass: "));
	lblSSAO.setTextOrigin(Text2D::TopLeft);
	lblSSAO.setPosition(glm::fvec2(15, 150));
	lblSSAO.setUpdate(
		[](UIWindow * win, UIElement* __this) -> void {
		UILabel* _this = (UILabel*)__this;

		_this->text.setString(std::string("SSAO pass: ") + std::to_string((double)Engine::r->ssaoTime / 1000.f) + " ms");
	});

	C_LABEL(lblLightPass, win);
	lblLightPass.setName("lightpass");
	lblLightPass.setStyle(styleA);
	lblLightPass.setString(String<32>("Shading pass: "));
	lblLightPass.setTextOrigin(Text2D::TopLeft);
	lblLightPass.setPosition(glm::fvec2(15, 190));
	lblLightPass.setUpdate(
		[](UIWindow * win, UIElement* __this) -> void {
		UILabel* _this = (UILabel*)__this;

		_this->text.setString(std::string("Shading pass: ") + std::to_string((double)Engine::r->lightPassTime / 1000.f) + " ms");
	});

	C_LABEL(lblScreen, win);
	lblScreen.setName("screenpass");
	lblScreen.setStyle(styleA);
	lblScreen.setString(String<32>("Screen pass: "));
	lblScreen.setTextOrigin(Text2D::TopLeft);
	lblScreen.setPosition(glm::fvec2(15, 230));
	lblScreen.setUpdate(
		[](UIWindow * win, UIElement* __this) -> void {
		UILabel* _this = (UILabel*)__this;

		_this->text.setString(std::string("Screen pass: ") + std::to_string((double)Engine::r->screenTime / 1000.f) + " ms");
	});

	C_LABEL(lblPhysics, win);
	lblPhysics.setName("physics");
	lblPhysics.setStyle(styleA);
	lblPhysics.setString(String<32>("Physics: "));
	lblPhysics.setTextOrigin(Text2D::TopLeft);
	lblPhysics.setPosition(glm::fvec2(15, 270));
	lblPhysics.setUpdate(
		[](UIWindow * win, UIElement* __this) -> void {
		UILabel* _this = (UILabel*)__this;

		_this->text.setString(std::string("Physics: ") + std::to_string((double)Engine::physicsTime / 1000.f) + " ms");
	});

	C_LABEL(lblRest, win);
	lblRest.setName("rest");
	lblRest.setStyle(styleA);
	lblRest.setString(String<32>("Frame time: "));
	lblRest.setTextOrigin(Text2D::TopLeft);
	lblRest.setPosition(glm::fvec2(15, 310));
	lblRest.setUpdate(
		[](UIWindow * win, UIElement* __this) -> void {
		UILabel* _this = (UILabel*)__this;

		_this->text.setString(std::string("Frame time: ") + std::to_string((double)Engine::frameTime / 1000.f) + " ms");
	});

	win->setMovable(true);
	win->addElement(lblGPUUpdatePtr);
	win->addElement(lblGBufferPtr);
	win->addElement(lblShadowPtr);
	win->addElement(lblSSAOPtr);
	win->addElement(lblLightPassPtr);
	win->addElement(lblScreenPtr);
	win->addElement(lblPhysicsPtr);
	win->addElement(lblRestPtr);

	return win;
}