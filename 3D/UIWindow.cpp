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

UIWindow::UIWindow(std::string pName, irect pWindowArea, int pBorderWidth, const Window* pParentWindow) : name(pName), parentWindow(pParentWindow), title(new UILabel(this)), borderWidth(pBorderWidth)
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

	proj = glm::ortho(0.f, float(windowArea.width), float(windowArea.height), 0.f, -1.f, 100.f);
	//proj = glm::ortho(0.f, float(windowArea.width), float(windowArea.height), 0.f, 0.f, 100.f);

	setTitle(name);
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
	renderTarget.clear(GL_COLOR_BUFFER_BIT, glm::fvec4(0.1, 0.1, 0.1, 0.9f));

	glBindVertexArray(vao);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glm::fvec4 col(0.2f, 0.2f, 0.2f, 0.0f);

	shader->use();
	shader->setColour(col);
	shader->setProjModel(proj);
	shader->sendUniforms();
	
	float innerAlpha = 0.9f;

	glBlendColor(0.f, 0.f, 0.f, innerAlpha); //Alpha is absolute alpha for inner area. RGB are not used.
	glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
	glBindTextureUnit(0, 0);
	glDrawArrays(GL_QUADS, 4, 4);

	glBindVertexArray(0);

	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

	if(hasTitle)
		title->draw();

	for (auto itr = elements.begin(); itr != elements.end(); ++itr)
	{
		(*itr).second->draw();
	}
	
	Engine::r->fboDefault.bind();
	glViewport(0, 0, Engine::window.getSizeX(), Engine::window.getSizeY());
	shader->use();

	col = glm::fvec4(0.f,0.f,0.f,0.9f);
	auto m = glm::ortho(0.f, (float)Engine::window.getSizeX(), (float)Engine::window.getSizeY(), 0.f, 0.f, 100.f);
	//auto m = glm::fmat4(0.f);
	shader->setColour(col);
	shader->setProjModel(m);
	shader->sendUniforms();
	
	renderTarget.textureAttachments[0].bind(0);

	glBindVertexArray(vao);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);

	glDrawArrays(GL_QUADS, 0, 4);


	glBindVertexArray(0);
}

void UIWindow::update()
{
	for (auto itr = elements.begin(); itr != elements.end(); ++itr)
	{
		(*itr).second->update();
	}

	if (dragging)
	{
		auto mp = Engine::window.getMousePosition();
		//mp.y = parentWindow->getSizeY() - mp.y;
		setWindowPosition(mp-clickedPos);
	}
}

void UIWindow::mouseDown(MouseEvent& pMouseEvent)
{
	auto mp = pMouseEvent.getPosition();
	//mp.y = parentWindow->getSizeY() - mp.y;
	if (!windowArea.contains(mp))
	{
		dragging = false;
		return;
	}

	bool canDrag = true;

	for (auto itr = elements.begin(); itr != elements.end(); ++itr)
	{
		auto rect = (*itr).second->getBounds();
		//auto mp = glm::ivec2(pMouseEvent.getUIWindowPosition((*itr).second->getParentWindow()).x, (*itr).second->getParentWindow()->getWindowRect().bot() - pMouseEvent.getUIWindowPosition((*itr).second->getParentWindow()).y);

		auto winMP = pMouseEvent.getUIWindowPosition((*itr).second->getParentWindow());

		if (!rect.contains(winMP))
			continue;

		(*itr).second->setClicked(true);
		(*itr).second->onMouseDown(pMouseEvent);
		canDrag = false;
	}

	if (canDrag)
	{
		clickedPos = mp - glm::ivec2(windowArea.left, windowArea.top);
		dragging = true;
	}
	else
		dragging = false;
}

void UIWindow::mouseUp(MouseEvent& pMouseEvent)
{
	for (auto itr = elements.begin(); itr != elements.end(); ++itr)
	{
		if ((*itr).second->isClicked())
		{
			(*itr).second->setClicked(false);
			(*itr).second->onMouseUp(pMouseEvent);
		}
	}
	dragging = false;
}

void UIWindow::keyDown(KeyEvent & pKeyEvent)
{
	for (auto itr = elements.begin(); itr != elements.end(); ++itr)
	{
		(*itr).second->onKeyDown(pKeyEvent);
	}
}

void UIWindow::keyUp(KeyEvent & pKeyEvent)
{
	for (auto itr = elements.begin(); itr != elements.end(); ++itr)
	{
		(*itr).second->onKeyUp(pKeyEvent);
	}
}

void UIWindow::checkMouseEnter(MouseEvent& pMouseEvent)
{
	auto mp = pMouseEvent.getPosition();
	//mp.y = parentWindow->getSizeY() - mp.y;

	for (auto itr = elements.begin(); itr != elements.end(); ++itr)
	{
		auto rect = (*itr).second->getBounds();
		//auto mp = glm::ivec2(pMouseEvent.getUIWindowPosition((*itr).second->getParentWindow()).x, (*itr).second->getParentWindow()->getWindowRect().height - pMouseEvent.getUIWindowPosition((*itr).second->getParentWindow()).y);

		auto winMP = pMouseEvent.getUIWindowPosition((*itr).second->getParentWindow());

		if (!rect.contains(winMP))
		{
			if ((*itr).second->isHovered())
			{
				(*itr).second->setHovered(false);
				(*itr).second->onMouseLeave(pMouseEvent);
				if ((*itr).second->isClicked())
					(*itr).second->setOffClick(true);
			}
			continue;
		}
		
		if (!(*itr).second->isHovered())
		{
			(*itr).second->setHovered(true);
			(*itr).second->onMouseEnter(pMouseEvent);
			if ((*itr).second->isClicked())
				(*itr).second->setOffClick(false);
		}
	}
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
	String<512> tit; tit.setToChars(name.c_str());
	title->setString(tit);
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