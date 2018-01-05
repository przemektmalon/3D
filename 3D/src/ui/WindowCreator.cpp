#include "WindowCreator.hpp"
#include "UIWindow.hpp"
#include "UIAllElements.hpp"
#include "File.hpp"
#include "rapidxml.hpp"
#include "Engine.hpp"
#include "XMLUtil.hpp"

UIWindow * WindowCreator::createWindow(std::string & specPath, std::function<void(UIWindow*)> scripts)
{
	XML xml;
	xml.parseFile(specPath);

	// Extract window info
	auto root = xml.getNode(&xml.doc,"window");

	auto name = xml.getString(xml.getAtt(root, "name"));
	auto pos = xml.toVec<glm::fvec2>(xml.getAtt(root, "pos"));
	auto size = xml.toVec<glm::fvec2>(xml.getAtt(root, "size"));
	auto border = xml.toVal<float>(xml.getAtt(root, "border"));
	auto movable = xml.getString(xml.getAtt(root, "movable"));


	UIWindow* w = new UIWindow(name, irect(pos.x,pos.y,size.x,size.y), border, &Engine::window);
	w->scriptsInitFunc = scripts;
	w->windowSpecFile = specPath;

	if (movable == "true")
		w->setMovable(true);
	else
		w->setMovable(false);

	// Extract all the window elements

	auto elNode = xml.getNode(root, "element");

	while (elNode)
	{
		auto type = xml.getString(xml.getAtt(elNode, "type"));

		/// TODO: completeness checking for mandatory attributes

		UIElement* el = nullptr;

		std::string tab, tabSection;

		if (type == "multitab")
		{
			el = new UIMultiTab(w);
			auto tab = (UIMultiTab*)el;

			auto currentTab = xml.getString(xml.getAtt(elNode, "currenttab"));

			tab->setName(xml.getString(xml.getAtt(elNode, "name")));
			tab->setPosition(xml.toVec<glm::fvec2>(xml.getAtt(elNode, "position")));
			tab->setSize(xml.toVec<glm::fvec2>(xml.getAtt(elNode, "size")));

			auto tabNode = xml.getNode(elNode, "tab");
			while (tabNode)
			{
				auto tabName = xml.getString(xml.getAtt(tabNode, "name"));
				tab->addTab(tabName);
				tabNode = xml.next(elNode, "tab");
			}

			tab->setCurrentTab(currentTab);
		}
		else if (type == "button")
		{
			el = new UIButton(w);
			auto but = (UIButton*)el;

			auto font = xml.getString(xml.getAtt(elNode, "font"));

			Font* fontPtr;
			if (font == "_NULL_")
				font = "clearsansb";
				
			fontPtr = Engine::assets.getFont(font);

			but->setName(xml.getString(xml.getAtt(elNode,"name")));
			but->setPosition(xml.toVec<glm::fvec2>(xml.getAtt(elNode, "position")));
			but->setSize(xml.toVec<glm::fvec2>(xml.getAtt(elNode, "size")));
			but->getText().setFont(fontPtr);
			but->setTextColour(xml.toVec<glm::fvec3>(xml.getAtt(elNode, "fontcolour")));
			but->getText().setCharSize(xml.toVal<int>(xml.getAtt(elNode, "fontsize")));
			but->setString(xml.getString(xml.getAtt(elNode, "text")));
		}
		else if (type == "label")
		{
			el = new UILabel(w);
			auto lbl = (UILabel*)el;

			auto font = xml.getString(elNode->first_attribute("font"));
			auto textOriginStr = xml.getString(elNode->first_attribute("textorigin"));

			Font* fontPtr;
			if (font == "_NULL_")
				font = "clearsansb";

			fontPtr = Engine::assets.getFont(font);

			Text2D::Origin textOrigin = Text2D::MiddleMiddle;
			if (textOriginStr != "_NULL_")
			{
				if (textOriginStr == "topleft")
					textOrigin = Text2D::TopLeft;
				else if (textOriginStr == "topright")
					textOrigin = Text2D::TopRight;
				else 
					{} /// TODO: the rest of the origins
			}

			lbl->setName(xml.getString(xml.getAtt(elNode, "name")));
			lbl->setPosition(xml.toVec<glm::fvec2>(xml.getAtt(elNode, "position")));
			lbl->setSize(xml.toVec<glm::fvec2>(xml.getAtt(elNode, "size")));
			lbl->setFont(fontPtr);
			lbl->setColour(xml.toVec<glm::fvec3>(xml.getAtt(elNode, "fontcolour")));
			lbl->setCharSize(xml.toVal<int>(xml.getAtt(elNode, "fontsize")));
			lbl->setString(xml.getString(xml.getAtt(elNode, "text")));
			lbl->setTextOrigin(textOrigin);
		}
		else if (type == "slider") /// TODO: better default options handling
		{
			el = new UISlider(w);
			auto sli = (UISlider*)el;

			auto pos = xml.toVec<glm::fvec2>(xml.getAtt(elNode, "position"));
			auto width = xml.toVal<float>(xml.getAtt(elNode, "width"));
			auto desc = xml.getString(xml.getAtt(elNode, "text"));
			auto font = xml.getString(xml.getAtt(elNode, "font"));
			auto fontSize = xml.toVal<int>(xml.getAtt(elNode, "fontsize"));
			auto colour = xml.toVec<glm::fvec3>(xml.getAtt(elNode, "fontcolour"));
			auto valType = xml.getString(xml.getAtt(elNode, "valuetype"));
			if (valType == "integer")
			{
				sli->valType = UISlider::Integer;
				auto limits = xml.toVec<glm::ivec2>(xml.getAtt(elNode, "limits"));
				sli->setLimits(limits);
				auto initial = xml.toVal<int>(xml.getAtt(elNode, "value"));
				sli->value.i = initial;
			}
			else
			{
				sli->valType = UISlider::Float;
				auto limits = xml.toVec<glm::fvec2>(xml.getAtt(elNode, "limits"));
				sli->setLimits(limits);
				auto initial = xml.toVal<float>(xml.getAtt(elNode, "value"));
				sli->value.f = initial;
			}

			sli->setName(xml.getString(xml.getAtt(elNode, "name")));
			sli->init(pos, width);
			sli->setDescription(desc);
		}
		else if (type == "input")
		{

		}
		else
		{

		}

		tab = xml.getString(elNode->first_attribute("tab"));
		tabSection = xml.getString(elNode->first_attribute("tabsection"));

		if (tab != "_NULL_" && tabSection != "_NULL_")
		{
			auto tabEl = (UIMultiTab*)w->getElement(tab);
			tabEl->addTabElement(tabSection, el);
		}
		else
		{
			w->addElement(el);
		}

		elNode = elNode->next_sibling("element");
	}

	scripts(w);

	return w;
}
