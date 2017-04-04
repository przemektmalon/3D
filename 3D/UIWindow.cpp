#include "UIWindow.h"
#include "UIElement.h"
#include "Engine.h"
#include "Renderer.h"
#include "Camera.h"
#include "Font.h"
#include "UILabel.h"
#include "Text.h"
#include "AssetManager.h"
#include "Mouse.h"

UIWindow::UIWindow(UIRect pWindowArea, int pBorderWidth, const Window* pParentWindow) : parentWindow(pParentWindow), title(new UILabel(this)), borderWidth(pBorderWidth)
{
	windowArea = pWindowArea;
	renderTarget.setResolution(glm::ivec2(windowArea.width, windowArea.height));
	renderTarget.attachTexture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0);

	title->init();
	title->setParentWindow(this);

	shader = Engine::r->shaderStore.getShader(String32("Shape2DShader"));

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

	glm::fvec4 col(0.35f, 0.35f, 0.35f, 0.8f);

	shader->use();	
	shader->setUniform(String64("projModel"), &proj);
	shader->setUniform(String64("colour"), &col);
	shader->sendUniforms();
	
	//glDisable(GL_BLEND);
	glBlendColor(0.f, 0.f, 0.f, 0.5f);
	glBlendFunc(GL_CONSTANT_ALPHA, GL_ZERO);
	glBindTextureUnit(0, 0);
	glDrawArrays(GL_QUADS, 4, 4);

	glBindVertexArray(0);

	//glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
	shader->setUniform(String64("colour"), &col);
	shader->setUniform(String64("projModel"), &m);
	shader->sendUniforms();
	
	renderTarget.textureAttachments[0].bind(0);

	glBindVertexArray(vao);

	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//or
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

/*void UIWindow::mouseDown(Event& pMouseEvent)
{
	glm::ivec2 mPos = Engine::window.mouse.getScreenPosition();
	
	mPos += -glm::ivec2(windowArea.left, windowArea.top);
	mPos.y = windowArea.height - mPos.y;
	for (auto itr = elements.begin(); itr != elements.end(); ++itr)
	{	
		(*itr)->onMouseDown(mPos);
		glm::ivec2 topLeft = (*itr)->getTopLeft();
		glm::ivec2 size = (*itr)->getSize();
		
		if (mPos.x > topLeft.x && mPos.x < topLeft.x + size.x && mPos.y > topLeft.y && mPos.y < topLeft.y + size.y)
		{
			
		}
	}
}*/

void UIWindow::mouseDown(MouseEvent& pMouseEvent)
{
	//glm::ivec2 mPos = pMouseEvent.getPosition();
	//mPos += -glm::ivec2(windowArea.left, windowArea.top);
	//mPos.y = windowArea.height - mPos.y;
	//pMouseEvent.setPosition(mPos);
	for (auto itr = elements.begin(); itr != elements.end(); ++itr)
	{
		(*itr)->onMouseDown(pMouseEvent);
	}
}

void UIWindow::mouseUp(MouseEvent& pMouseEvent)
{
	//glm::ivec2 mPos = pMouseEvent.getPosition();
	//mPos += -glm::ivec2(windowArea.left, windowArea.top);
	//mPos.y = windowArea.height - mPos.y;
	//pMouseEvent.setPosition(mPos);
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

void UIWindow::setTitle(std::string pTitle)
{
	//Font* font = new Font();//TODO: GET FROM RES MANAGER
	//font->load("res/fonts/clear-sans/ClearSans-Regular.ttf");
	//font->loadGlyphs(14);

	title->setParentWindow(this);

	title->setFont(Engine::assets.getFont("clearsans"));
	title->setCharSize(14);
	title->setColour(glm::fvec3(0.f, 1.f, 1.f));
	title->setWindowSize(glm::ivec2(windowArea.width, windowArea.height));
	title->setWindowOrigin(Text2D::TopLeft);
	String<32> tit; tit.setToChars(pTitle.c_str());
	title->setString(tit);
	title->setTextOrigin(glm::ivec2(title->getBoundingBox().width / 2.f, 18));
	title->setPosition(glm::fvec2(windowArea.width / 2.f - 5.f, -borderWidth - titleWidth));
}

void UIWindow::addElement(UIElement * pEl)
{
	pEl->setParentWindow(this); elements.push_back(pEl);
}
