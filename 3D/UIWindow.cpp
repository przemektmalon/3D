#include "UIWindow.hpp"
#include "UIElement.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "Font.hpp"
#include "UILabel.hpp"
#include "Text.hpp"
#include "AssetManager.hpp"
#include "Mouse.hpp"

UIWindow::UIWindow(UIRect pWindowArea, int pBorderWidth, const Window* pParentWindow) : parentWindow(pParentWindow), title(new UILabel(this)), borderWidth(pBorderWidth)
{
	windowArea = pWindowArea;
	renderTarget.setResolution(glm::ivec2(windowArea.width, windowArea.height));
	renderTarget.attachTexture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0);

	title->init();
	title->setParentWindow(this);

	shader = (Shape2DShader*)Engine::r->shaderStore.getShader(String32("Shape2DShader"));

	glCreateVertexArrays(1, &vao);
	glCreateBuffers(1, &vbo);
	
	updateWindowVBO();

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	shader->use();

	auto posAttrib = glGetAttribLocation(shader->getGLID(), "p");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

	auto texAttrib = glGetAttribLocation(shader->getGLID(), "t");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(float)));

	shader->stop();	

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	proj = glm::ortho(0.f, float(windowArea.width), 0.f, float(windowArea.height), -1.f, 100.f);
	//proj = glm::ortho(0.f, float(windowArea.width), float(windowArea.height), 0.f, 0.f, 100.f);
}

UIWindow::~UIWindow()
{

}

void UIWindow::draw()
{
	//glm::fmat4 m = glm::ortho(0.f, float(parentWindow->getSizeX()), 0.f, float(parentWindow->getSizeY()), 0.f, 100.f);

	glViewport(0, 0, windowArea.width, windowArea.height);

	renderTarget.bind();
	//renderTarget.textureAttachments[0].bind();
	renderTarget.clear(GL_COLOR_BUFFER_BIT, glm::fvec4(0.05, 0.05, 0.05, 0.65f));

	glBindVertexArray(vao);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glm::fvec4 col(0.2f, 0.2f, 0.2f, 0.0f);

	shader->use();
	shader->setColour(col);
	shader->setProjModel(proj);
	shader->sendUniforms();
	
	float innerAlpha = 0.8f;

	glBlendColor(0.f, 0.f, 0.f, innerAlpha); //Alpha is absolute alpha for inner area. RGB are not used.
	glBlendFunc(GL_CONSTANT_ALPHA, GL_ZERO);
	glBindTextureUnit(0, 0);
	glDrawArrays(GL_QUADS, 4, 4);

	glBindVertexArray(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if(hasTitle)
		title->draw();

	for (auto itr = elements.begin(); itr != elements.end(); ++itr)
	{
		(*itr)->draw();
	}
	
	Engine::r->fboDefault.bind();
	glViewport(0, 0, Engine::window.getSizeX(), Engine::window.getSizeY());
	shader->use();

	col = glm::fvec4(0.f,0.f,0.f,0.f);
	auto m = glm::ortho(0.f, (float)Engine::window.getSizeX(), 0.f, (float)Engine::window.getSizeY(), 0.f, 100.f);
	//auto m = glm::fmat4(0.f);
	shader->setColour(col);
	shader->setProjModel(m);
	shader->sendUniforms();
	
	renderTarget.textureAttachments[0].bind(0);

	glBindVertexArray(vao);

	glEnable(GL_BLEND);
	//glDisable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//or
	glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);


	glBindVertexArray(0);
}

void UIWindow::update()
{
	for (auto itr = elements.begin(); itr != elements.end(); ++itr)
	{
		(*itr)->update();
	}
}

void UIWindow::mouseDown(MouseEvent& pMouseEvent)
{
	for (auto itr = elements.begin(); itr != elements.end(); ++itr)
	{
		(*itr)->onMouseDown(pMouseEvent);
	}
}

void UIWindow::mouseUp(MouseEvent& pMouseEvent)
{
	for (auto itr = elements.begin(); itr != elements.end(); ++itr)
	{
		(*itr)->onMouseUp(pMouseEvent);
	}
}

void UIWindow::keyDown(KeyEvent & pKeyEvent)
{
	for (auto itr = elements.begin(); itr != elements.end(); ++itr)
	{
		(*itr)->onKeyDown(pKeyEvent);
	}
}

void UIWindow::keyUp(KeyEvent & pKeyEvent)
{
	for (auto itr = elements.begin(); itr != elements.end(); ++itr)
	{
		(*itr)->onKeyUp(pKeyEvent);
	}
}

void UIWindow::mouseHover(MouseEvent& pMouseEvent)
{
	for (auto itr = elements.begin(); itr != elements.end(); ++itr)
	{
		(*itr)->onHover(pMouseEvent);
	}
}

void UIWindow::setTitle(std::string pTitle)
{
	if (pTitle.size() == 0)
	{
		hasTitle = false;
		return;
	}

	hasTitle = true;

	title->setParentWindow(this);

	title->setFont(Engine::assets.getFont("clearsans"));
	title->setCharSize(14);
	title->setColour(glm::fvec3(0.f, 1.f, 1.f));
	title->setWindowSize(glm::ivec2(windowArea.width, windowArea.height));
	title->setWindowOrigin(Text2D::TopLeft);
	String<512> tit; tit.setToChars(pTitle.c_str());
	title->setString(tit);
	//title->setTextOrigin(glm::ivec2(title->getBoundingBox().width / 2.f, title->getBoundingBox().height));
	title->setTextOrigin(Text2D::MiddleMiddle);
	//title->setPosition(glm::fvec2(windowArea.width / 2.f - 5.f, -borderWidth - titleWidth));
	title->setPosition(glm::fvec2(windowArea.width / 2.f, 0));
	//title->setPosition(glm::fvec2(0, 0));

	updateWindowVBO();
}

void UIWindow::addElement(UIElement * pEl)
{
	pEl->setParentWindow(this); elements.push_back(pEl);
}

void UIWindow::updateWindowVBO()
{
	if (hasTitle)
		if(borderWidth == 0)
			titleWidth = title->getBoundingBox().height + 10;
		else
			titleWidth = title->getBoundingBox().height + 4;
	else
		titleWidth = 0;

	float windowVerts[40] = {
		//OUTSIDE AREA
		windowArea.left, parentWindow->getSizeY() - windowArea.top,0.f,  0.0f, 1.0f,//TL
		windowArea.left + windowArea.width, parentWindow->getSizeY() - windowArea.top,0.f,  1.0f, 1.0f,//TR
		windowArea.left + windowArea.width, parentWindow->getSizeY() - windowArea.top - windowArea.height,0.f,  1.0f, 0.0f,//BR
		windowArea.left, parentWindow->getSizeY() - windowArea.top - windowArea.height,0.f,  0.0f, 0.0f,//BL

		//INSIDE AREA
		borderWidth, windowArea.height - borderWidth - titleWidth ,0.f,0,1,
		windowArea.width - borderWidth, windowArea.height - borderWidth - titleWidth,0.f,1,1,
		windowArea.width - borderWidth, borderWidth,0.f,1,0,
		borderWidth,borderWidth,0.f,0,0

		/*borderWidth,	windowArea.height - borderWidth - 20,  0.0f, 1.0f,//TL
		windowArea.width - borderWidth, windowArea.height - 20 - borderWidth,  1.0f, 1.0f,//TR
		windowArea.width - borderWidth, borderWidth,  1.0f, 0.0f,//BR
		borderWidth, borderWidth,  0.0f, 0.0f,//BL*/
	};

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(windowVerts), windowVerts, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(windowVerts), windowVerts);
	//glBufferSubData(GL_ARRAY_BUFFER, sizeof(windowVerts), sizeof(elementVerts), elementVerts);
}