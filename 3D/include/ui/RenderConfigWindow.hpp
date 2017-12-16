#pragma once
#include "UIWindow.hpp"
#include "UIAllElements.hpp"
#include "Text.hpp"
#include <string>

#include "Engine.hpp"

UIWindow* createRenderConfigWindow()
{
	auto win = new UIWindow("Render Config", irect(0, 0, 300, 350), 4, &Engine::window);

	Text2D::TextStyle styleA(Engine::assets.getFont("clearsansb"), 16);
	styleA.textOriginPreDef = Text2D::MiddleMiddle;

	C_LABEL(lblFPS, win);
	lblFPS.setName("fps");
	lblFPS.setStyle(styleA);
	lblFPS.setString("FPS: ");
	lblFPS.setPosition(glm::fvec2(15, 30));
	lblFPS.setUpdate(
		[](UIWindow * win, UIElement* __this) -> void {
		UILabel* _this = (UILabel*)__this;

		_this->text.setString(std::string("FPS: ") + std::to_string(1.f / Engine::deltaTime.getSecondsf()));
	});

	C_MULTITAB(tab, win);
	tab.setName("tab");
	tab.setPosition(glm::fvec2(15, 60));
	tab.setDimensions(glm::fvec2(270, 280));
	tab.addTab("Render");
	tab.addTab("SSAO");
	tab.addTab("GBuffer");
	tab.addTab("Resolution");

	C_SLIDER(sliSSAORad, win);
	sliSSAORad.setName("ssaorad");
	sliSSAORad.setDescription("SSAO Radius");
	sliSSAORad.valType = UISlider::Float;
	sliSSAORad.setLimits(glm::fvec2(0.1f, 40.f));
	sliSSAORad.value.f = 15.f;
	sliSSAORad.init(glm::fvec2(20, 90), 260);
	sliSSAORad.setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UISlider* _this = (UISlider*)__this;
		Engine::cfg.render.ssao.setSampleRadius(_this->value.f);
	});

	C_SLIDER(sliSSAOInt, win);
	sliSSAOInt.setName("ssaoint");
	sliSSAOInt.setDescription("SSAO Intensity");
	sliSSAOInt.valType = UISlider::Float;
	sliSSAOInt.setLimits(glm::fvec2(0.1f, 40.f));
	sliSSAOInt.value.f = 4.f;
	sliSSAOInt.init(glm::fvec2(20, 130), 260);
	sliSSAOInt.setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UISlider* _this = (UISlider*)__this;
		Engine::cfg.render.ssao.setIntensity(_this->value.f);
	});

	C_SLIDER(sliSSAOSam, win);
	sliSSAOSam.setName("ssaosamples");
	sliSSAOSam.setDescription("SSAO Samples");
	sliSSAOSam.valType = UISlider::Integer;
	sliSSAOSam.setLimits(glm::ivec2(5, 200));
	sliSSAOSam.value.i = 50;
	sliSSAOSam.init(glm::fvec2(20, 170), 260);
	sliSSAOSam.setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UISlider* _this = (UISlider*)__this;
		Engine::cfg.render.ssao.setSamples(_this->value.i);
	});

	C_SLIDER(sliSSAOSpi, win);
	sliSSAOSpi.setName("ssaospirals");
	sliSSAOSpi.setDescription("SSAO Spirals");
	sliSSAOSpi.valType = UISlider::Integer;
	sliSSAOSpi.setLimits(glm::ivec2(0, 200));
	sliSSAOSpi.value.i = 9;
	sliSSAOSpi.init(glm::fvec2(20, 210), 260);
	sliSSAOSpi.setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UISlider* _this = (UISlider*)__this;
		Engine::cfg.render.ssao.setSpiralTurns(_this->value.i);
	});

	C_BUTTON(butToggleWire, win);
	butToggleWire.setName("button");
	butToggleWire.setSize(glm::fvec2(260, 30));
	butToggleWire.getText().setStyle(styleA);
	butToggleWire.getText().setCharSize(18);
	butToggleWire.setString("Toggle wireframe");
	butToggleWire.setPosition(glm::fvec2(20, 90));
	butToggleWire.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.toggleDrawWireframe();
	});

	C_BUTTON(butToggleVSync, win);
	butToggleVSync.setName("vsync");
	butToggleVSync.setSize(glm::fvec2(260, 30));
	butToggleVSync.getText().setStyle(styleA);
	butToggleVSync.getText().setCharSize(18);
	butToggleVSync.setString("Toggle vertical sync");
	butToggleVSync.setPosition(glm::fvec2(20, 130));
	butToggleVSync.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.toggleVSync();
	});

	C_BUTTON(butReloadShaders, win);
	butReloadShaders.setName("shaders");
	butReloadShaders.setSize(glm::fvec2(260, 30));
	butReloadShaders.getText().setStyle(styleA);
	butReloadShaders.getText().setCharSize(18);
	butReloadShaders.setString("Reload shaders");
	butReloadShaders.setPosition(glm::fvec2(20, 170));
	butReloadShaders.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.reloadAllShaders();
	});

	C_BUTTON(butScreenshot, win);
	butScreenshot.setName("screenshot");
	butScreenshot.setSize(glm::fvec2(260, 30));
	butScreenshot.getText().setStyle(styleA);
	butScreenshot.getText().setCharSize(18);
	butScreenshot.setString("Screenshot");
	butScreenshot.setPosition(glm::fvec2(20, 210));
	butScreenshot.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.screenshot();
	});

	C_SLIDER(sliExposure, win);
	sliExposure.setName("exposure");
	sliExposure.setDescription("Exposure");
	sliExposure.valType = UISlider::Float;
	sliExposure.setLimits(glm::fvec2(0.1f, 70.f));
	sliExposure.value.f = 50.f;
	sliExposure.init(glm::fvec2(20, 250), 260);
	sliExposure.setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UISlider* _this = (UISlider*)__this;
		Engine::cfg.render.camera.setExposure(_this->value.f);
	});

	C_SLIDER(sliFOV, win);
	sliFOV.setName("fov");
	sliFOV.setDescription("FOV");
	sliFOV.valType = UISlider::Float;
	sliFOV.setLimits(glm::fvec2(10.f, 150.f));
	sliFOV.value.f = 90.f;
	sliFOV.init(glm::fvec2(20, 290), 260);
	sliFOV.setUpdate([](UIWindow* win, UIElement* __this) -> void {
		UISlider* _this = (UISlider*)__this;
		Engine::cfg.render.camera.setFOV(_this->value.f);
	});

	C_BUTTON(butShaded, win);
	butShaded.setName("shaded");
	butShaded.setSize(glm::fvec2(260, 30));
	butShaded.getText().setStyle(styleA);
	butShaded.getText().setCharSize(18);
	butShaded.setString("Shaded");
	butShaded.setPosition(glm::fvec2(20, 90));
	butShaded.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setRenderMode(0);
	});

	C_BUTTON(butAlbedo, win);
	butAlbedo.setName("albedo");
	butAlbedo.setSize(glm::fvec2(260, 30));
	butAlbedo.getText().setStyle(styleA);
	butAlbedo.getText().setCharSize(18);
	butAlbedo.setString("Albedo");
	butAlbedo.setPosition(glm::fvec2(20, 130));
	butAlbedo.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setRenderMode(1);
	});

	C_BUTTON(butNormal, win);
	butNormal.setName("normal");
	butNormal.setSize(glm::fvec2(260, 30));
	butNormal.getText().setStyle(styleA);
	butNormal.getText().setCharSize(18);
	butNormal.setString("Normal");
	butNormal.setPosition(glm::fvec2(20, 170));
	butNormal.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setRenderMode(2);
	});

	C_BUTTON(butSSAO, win);
	butSSAO.setName("ssao");
	butSSAO.setSize(glm::fvec2(260, 30));
	butSSAO.getText().setStyle(styleA);
	butSSAO.getText().setCharSize(18);
	butSSAO.setString("SSAO");
	butSSAO.setPosition(glm::fvec2(20, 210));
	butSSAO.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setRenderMode(3);
	});

	C_BUTTON(butDepth, win);
	butDepth.setName("depth");
	butDepth.setSize(glm::fvec2(260, 30));
	butDepth.getText().setStyle(styleA);
	butDepth.getText().setCharSize(18);
	butDepth.setString("Depth");
	butDepth.setPosition(glm::fvec2(20, 250));
	butDepth.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setRenderMode(4);
	});

	C_BUTTON(butRes0, win);
	butRes0.setName("res0");
	butRes0.setSize(glm::fvec2(125, 30));
	butRes0.getText().setStyle(styleA);
	butRes0.getText().setCharSize(18);
	butRes0.setString("3840x2160");
	butRes0.setPosition(glm::fvec2(20, 90));
	butRes0.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setResolution(0);
	});

	C_BUTTON(butRes1, win);
	butRes1.setName("res1");
	butRes1.setSize(glm::fvec2(125, 30));
	butRes1.getText().setStyle(styleA);
	butRes1.getText().setCharSize(18);
	butRes1.setString("1920x1080");
	butRes1.setPosition(glm::fvec2(155, 90));
	butRes1.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setResolution(1);
	});

	C_BUTTON(butRes2, win);
	butRes2.setName("res2");
	butRes2.setSize(glm::fvec2(125, 30));
	butRes2.getText().setStyle(styleA);
	butRes2.getText().setCharSize(18);
	butRes2.setString("1600x900");
	butRes2.setPosition(glm::fvec2(20, 130));
	butRes2.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setResolution(2);
	});

	C_BUTTON(butRes3, win);
	butRes3.setName("res3");
	butRes3.setSize(glm::fvec2(125, 30));
	butRes3.getText().setStyle(styleA);
	butRes3.getText().setCharSize(18);
	butRes3.setString("1546x864");
	butRes3.setPosition(glm::fvec2(155, 130));
	butRes3.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setResolution(3);
	});

	C_BUTTON(butRes5, win);
	butRes5.setName("res5");
	butRes5.setSize(glm::fvec2(125, 30));
	butRes5.getText().setStyle(styleA);
	butRes5.getText().setCharSize(18);
	butRes5.setString("1280x720");
	butRes5.setPosition(glm::fvec2(20, 170));
	butRes5.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setResolution(5);
	});

	C_BUTTON(butRes6, win);
	butRes6.setName("res6");
	butRes6.setSize(glm::fvec2(125, 30));
	butRes6.getText().setStyle(styleA);
	butRes6.getText().setCharSize(18);
	butRes6.setString("1024x576");
	butRes6.setPosition(glm::fvec2(155, 170));
	butRes6.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setResolution(6);
	});

	C_BUTTON(butRes7, win);
	butRes7.setName("res7");
	butRes7.setSize(glm::fvec2(125, 30));
	butRes7.getText().setStyle(styleA);
	butRes7.getText().setCharSize(18);
	butRes7.setString("960x540");
	butRes7.setPosition(glm::fvec2(20, 210));
	butRes7.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setResolution(7);
	});

	C_BUTTON(butRes8, win);
	butRes8.setName("res8");
	butRes8.setSize(glm::fvec2(125, 30));
	butRes8.getText().setStyle(styleA);
	butRes8.getText().setCharSize(18);
	butRes8.setString("848x480");
	butRes8.setPosition(glm::fvec2(155, 210));
	butRes8.setOnMouseUp([](UIWindow* win, UIElement* __this, MouseEvent& ke) -> void {
		UIButton* _this = (UIButton*)__this;
		Engine::cfg.render.setResolution(8);
	});

	win->setMovable(true);
	win->addElement(lblFPSPtr);

	tab.addTabElement("SSAO", sliSSAORadPtr);
	tab.addTabElement("SSAO", sliSSAOIntPtr);
	tab.addTabElement("SSAO", sliSSAOSamPtr);
	tab.addTabElement("SSAO", sliSSAOSpiPtr);

	tab.addTabElement("Render", butToggleWirePtr);
	tab.addTabElement("Render", butToggleVSyncPtr);
	tab.addTabElement("Render", butReloadShadersPtr);
	tab.addTabElement("Render", butScreenshotPtr);
	tab.addTabElement("Render", sliExposurePtr);
	tab.addTabElement("Render", sliFOVPtr);

	tab.addTabElement("GBuffer", butShadedPtr);
	tab.addTabElement("GBuffer", butAlbedoPtr);
	tab.addTabElement("GBuffer", butNormalPtr);
	tab.addTabElement("GBuffer", butSSAOPtr);
	tab.addTabElement("GBuffer", butDepthPtr);

	tab.addTabElement("Resolution", butRes0Ptr);
	tab.addTabElement("Resolution", butRes1Ptr);
	tab.addTabElement("Resolution", butRes2Ptr);
	tab.addTabElement("Resolution", butRes3Ptr);
	tab.addTabElement("Resolution", butRes5Ptr);
	tab.addTabElement("Resolution", butRes6Ptr);
	tab.addTabElement("Resolution", butRes7Ptr);
	tab.addTabElement("Resolution", butRes8Ptr);

	tab.setCurrentTab("Render");

	win->addElement(tabPtr);

	tab.forceUpdateAll();

	return win;
}