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

UIWindow::UIWindow(UIRect pWindowArea, int pBorderWidth, const Window* pParentWindow) : parentWindow(pParentWindow), title(new UILabel()), borderWidth(pBorderWidth)
{
	windowArea = pWindowArea;
	renderTarget.setResolution(glm::ivec2(windowArea.width, windowArea.height));
	renderTarget.attachTexture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0);

	title->text.init();

	shader.initialise();

	glCreateVertexArrays(1, &vao);
	glCreateBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, NULL, GL_STATIC_DRAW);

	updateWindowVBO();

	shader.use();


	auto posAttrib = glGetAttribLocation(shader(), "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	auto texAttrib = glGetAttribLocation(shader(), "texCoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	proj = glm::ortho(0.f, float(windowArea.width), 0.f, float(windowArea.height), 0.99f, 100.f);

}

UIWindow::~UIWindow()
{
}

void UIWindow::draw()
{
	renderTarget.bind();
	renderTarget.textureAttachments[0].bind();
	renderTarget.clear(GL_COLOR_BUFFER_BIT, glm::fvec4(0.1, 0.1, 0.1, 0.3f));

	glBindVertexArray(vao);

	shader.use();
	glm::fmat4 m = glm::ortho(0.f, float(parentWindow->getSizeX()), 0.f, float(parentWindow->getSizeY()), 0.99f, 100.f);
	shader.setProj(m);
	shader.setModel(glm::fmat4());
	shader.setView(glm::fmat4());
	glm::fvec4 col(0.05f, 0.05f, 0.05f, 0.6f);
	glUniform4fv(glGetUniformLocation(shader(), "colour"), 1, glm::value_ptr(col));
	glBlendFunc(GL_SRC_COLOR, GL_ONE);
	glBindTextureUnit(0, 0);
	glDrawArrays(GL_QUADS, 4, 4);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glBindVertexArray(0);

	glViewport(0, 0, windowArea.width, windowArea.height);
	title->draw();
	for (auto itr = elements.begin(); itr != elements.end(); ++itr)
	{
		(*itr)->draw();
	}

	Engine::r->fboDefault.bind();
	glViewport(0, 0, Engine::window.getSizeX(), Engine::window.getSizeY());
	glBindVertexArray(vao);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);

	shader.use();
	col = glm::fvec4(0.f);
	glUniform4fv(glGetUniformLocation(shader(), "colour"), 1, glm::value_ptr(col));
	//glm::fmat4 m = glm::ortho(0.f, 1280.f, 0.f, 720.f, 0.99f, 100.f);
	shader.setProj(m);
	shader.setModel(glm::fmat4());
	shader.setView(glm::fmat4());
	renderTarget.textureAttachments[0].bind(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//or
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);

	glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void UIWindow::update()
{
	for (auto itr = elements.begin(); itr != elements.end(); ++itr)
	{
		(*itr)->update();
	}
}

void UIWindow::mouseDown()
{
	for (auto itr = elements.begin(); itr != elements.end(); ++itr)
	{
		glm::ivec2 mPos = Engine::window.mouse.getScreenPosition() - glm::ivec2(windowArea.left, windowArea.top) + borderWidth + glm::ivec2(0, titleWidth);
		glm::ivec2 topLeft = (*itr)->getTopLeft();
		glm::ivec2 size = (*itr)->getSize();

		if (mPos.x > topLeft.x && mPos.x < topLeft.x + size.x && mPos.y > topLeft.y && mPos.y < topLeft.y + size.y)
		{
			(*itr)->mouseDown();
		}
	}
}

void UIWindow::setTitle(std::string pTitle)
{
	//Font* font = new Font();//TODO: GET FROM RES MANAGER
	//font->load("res/fonts/clear-sans/ClearSans-Regular.ttf");
	//font->loadGlyphs(14);

	//title->text.init();
	title->setParentWindow(this);
	title->text.setFont(&Engine::assets.getFont("clearsans"));
	title->text.setCharSize(14);
	title->text.setColour(glm::fvec3(1.f, 1.f, 1.f));
	title->text.setWindowSize(glm::ivec2(windowArea.width, windowArea.height));
	title->text.setWindowOrigin(Text2D::TopLeft);
	title->text.setString("DEBUG OPTIONS");
	title->text.setTextOrigin(glm::ivec2(title->text.getBoundingBox().width / 2.f, 18));
	title->text.setPosition(glm::fvec2(windowArea.width / 2.f - 5.f, 0));
}

void UIWindow::addElement(UIElement * pEl)
{
	pEl->setParentWindow(this); elements.push_back(pEl);
}
