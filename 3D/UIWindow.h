#pragma once
#include "UIUtil.h"
#include "Framebuffer.h"
#include "Shape2DShader.h"
#include "Window.h"

class Font;
class UIElement;
class UILabel;

class UIWindow
{
public:
	UIWindow(UIRect pWindowArea, int pBorderWidth, const Window* pParentWindow);
	~UIWindow();

	void draw();
	const GLTexture2D& getTexture() { return renderTarget.textureAttachments[0]; }

	void setTitle(std::string pTitle);
	void addElement(UIElement* pEl);

	glm::fmat4& getProj() { return proj; }

	void resizeWindow(u32 newX, u32 newY)
	{
		windowArea.width = newX - windowArea.left;
		windowArea.height = newY - windowArea.top;
	}

	void moveWindow(u32 newX, u32 newY)
	{
		windowArea.left = newX;
		windowArea.top = newY;
	}

	void updateWindowVBO()
	{
		float windowVerts[32] = {
			windowArea.left, parentWindow->getSizeY() - windowArea.top,  0.0f, 1.0f,//TL
			windowArea.left + windowArea.width,  parentWindow->getSizeY() - windowArea.top,  1.0f, 1.0f,//TR
			windowArea.left + windowArea.width,parentWindow->getSizeY() - windowArea.top - windowArea.height,  1.0f, 0.0f,//BR
			windowArea.left, parentWindow->getSizeY() - windowArea.top - windowArea.height,  0.0f, 0.0f,//BL

			borderWidth,windowArea.height - borderWidth - 20.f ,0,1,
			windowArea.width - borderWidth, windowArea.height - borderWidth - 20.f,1,1,
			windowArea.width - borderWidth, borderWidth,1,0,
			borderWidth,borderWidth,0,0

			/*borderWidth,	windowArea.height - borderWidth - 20,  0.0f, 1.0f,//TL
			windowArea.width - borderWidth, windowArea.height - 20 - borderWidth,  1.0f, 1.0f,//TR
			windowArea.width - borderWidth, borderWidth,  1.0f, 0.0f,//BR
			borderWidth, borderWidth,  0.0f, 0.0f,//BL*/
		};

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(windowVerts), windowVerts, GL_STATIC_DRAW);
		//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(windowVerts), windowVerts);
		//glBufferSubData(GL_ARRAY_BUFFER, sizeof(windowVerts), sizeof(elementVerts), elementVerts);
	}

	UIRect getWindowArea() { return windowArea; }

	void drag(glm::ivec2 delta)
	{
		windowArea.left += delta.x;
		windowArea.top += delta.y;
		updateWindowVBO();//TODO: DONT UPDATE VBO, CHANGE TRANSFORM(MODEL) MATRIX
	}

	void setWindowPosition(glm::ivec2 pos)
	{
		windowArea.left = pos.x;
		windowArea.top = pos.y;
		updateWindowVBO();
	}

private:

	glm::ivec2 position;

	UIRect windowArea;
	UIRect elementArea;
	int borderWidth;

	Framebuffer renderTarget;

	std::vector<UIElement*> elements;
	UILabel* title;

	Shape2DShader shader;

	GLuint vao;
	GLuint vbo;

	glm::fmat4 proj;
	const Window* parentWindow;




};
