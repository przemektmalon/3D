#include "ui/UIWindow.hpp"
#include "ui/UIElement.hpp"
#include "Engine.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "Font.hpp"
#include "ui/UILabel.hpp"
#include "Text.hpp"
#include "AssetManager.hpp"
#include "Mouse.hpp"
#include "ui/UIMultiTab.hpp"

UIWindow::UIWindow(std::string pName, irect pWindowArea, int pBorderWidth, const Window* pParentWindow) : name(pName), parentWindow(pParentWindow), title(new UILabel(this)), borderWidth(pBorderWidth)
{
	windowArea = pWindowArea;

	renderTarget.setResolution(glm::ivec2(windowArea.width, windowArea.height));
	renderTarget.attachTexture(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0);

	title->init();
	title->setParentWindow(this);

	shader = (Shape2DShader*)Engine::r->shaderStore.getShader("Shape2DShader");
	shaderNoTex = (Shape2DShaderNoTex*)Engine::r->shaderStore.getShader("Shape2DShaderNoTex");

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

	proj = glm::ortho(0.f, float(windowArea.width), float(windowArea.height), 0.f, -1.f, 100.f);

	setTitle(name);
	movable = true;
}

UIWindow::~UIWindow()
{

}

void UIWindow::draw()
{
	glViewport(0, 0, windowArea.width, windowArea.height);

	renderTarget.bind();
	renderTarget.clear(GL_COLOR_BUFFER_BIT, glm::fvec4(0.1f, 0.1f, 0.1f, 0.6f));

	glBindVertexArray(vao);
	
	glm::fvec4 col(0.2f, 0.2f, 0.2f, 0.85f);

	shaderNoTex->use();
	shaderNoTex->setColour(col);
	shaderNoTex->setProjModel(proj);
	shaderNoTex->sendUniforms();
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_QUADS, 4, 4);

	if(hasTitle)
		title->draw();

	UIElementContainer::draw();
	
	DefaultFBO::bind();
	glViewport(0, 0, Engine::window.getSizeX(), Engine::window.getSizeY());
	shader->use();

	shader->setColour(glm::fvec4(0, 0, 0, 0));
	shader->setProjModel(parentWindow->getOrthoProj());
	shader->sendUniforms();
	
	renderTarget.textureAttachments[0].bind(0);

	glBindVertexArray(vao);

	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_COLOR, GL_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);

	glBindVertexArray(0);
}

void UIWindow::update()
{
	UIElementContainer::update();

	if (dragging)
	{
		auto mp = Engine::window.getMousePosition();
		setWindowPosition(mp-clickedPos);
	}
}

bool UIWindow::mouseDown(MouseEvent& pMouseEvent)
{
	auto mp = pMouseEvent.getPosition();
	if (!windowArea.contains(mp))
	{
		dragging = false;
		return false;
	}

	bool canDrag = UIElementContainer::mouseDown(pMouseEvent);

	if (canDrag)
	{
		clickedPos = mp - glm::ivec2(windowArea.left, windowArea.top);
		dragging = true;
	}
	else
		dragging = false;
	return dragging;
}

void UIWindow::mouseUp(MouseEvent& pMouseEvent)
{
	UIElementContainer::mouseUp(pMouseEvent);
	dragging = false;
}

void UIWindow::setTitle(std::string pTitle)
{
	if (pTitle.size() == 0)
	{
		hasTitle = false;
		return;
	}

	name = pTitle;

	hasTitle = true;

	title->setParentWindow(this);

	title->setFont(Engine::assets.getFont("clearsansb"));
	title->setCharSize(14);
	title->setColour(glm::fvec3(0.f, 1.f, 1.f));
	title->setString(pTitle);
	title->setTextOrigin(Text2D::TopMiddle);
	title->setPosition(glm::fvec2(windowArea.width / 2.f, 0));

	updateWindowVBO();
}

void UIWindow::addElement(UIElement * pEl)
{
	pEl->setParentWindow(this); 
	elements.insert(std::make_pair(pEl->getName(), pEl));
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
		windowArea.left, windowArea.top, 0.f, 0.0f, 1.0f,//TL
		windowArea.left + windowArea.width, windowArea.top, 0.f, 1.0f, 1.0f,//TR
		windowArea.left + windowArea.width, windowArea.top + windowArea.height, 0.f, 1.0f, 0.0f,//BR
		windowArea.left, windowArea.top + windowArea.height, 0.f, 0.0f, 0.0f,//BL

		//INSIDE AREA
		
		/// TODO: These should not have clockwise winding for conventions sake (culling usually done on clockwise polys)

		borderWidth, borderWidth + titleWidth ,0.f,0,1,//TL
		windowArea.width - borderWidth, borderWidth + titleWidth,0.f,1,1,//TR
		windowArea.width - borderWidth, windowArea.height - borderWidth,0.f,1,0,//BR
		borderWidth, windowArea.height - borderWidth,0.f,0,0//BL
	};

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(windowVerts), windowVerts, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}