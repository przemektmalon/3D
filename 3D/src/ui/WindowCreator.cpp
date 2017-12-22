#include "WindowCreator.hpp"
#include "UIWindow.hpp"
#include "UIAllElements.hpp"
#include "File.hpp"
#include "rapidxml.hpp"
#include "Engine.hpp"
#include "XMLUtil.hpp"

UIWindow * WindowCreator::createWindow(std::string & specPath, std::function<void(UIWindow*)> scripts)
{
	// Set up xml parser

	File windowSpecFile;
	windowSpecFile.open(specPath, File::in);

	std::string xmlString;

	windowSpecFile.fstream().seekg(0, std::ios::end);
	xmlString.reserve(windowSpecFile.fstream().tellg());
	windowSpecFile.fstream().seekg(0, std::ios::beg);

	xmlString.assign((std::istreambuf_iterator<char>(windowSpecFile.fstream())), std::istreambuf_iterator<char>());

	rapidxml::xml_document<> doc;
	doc.parse<0>(const_cast<char*>(xmlString.c_str()));

	rapidxml::xml_node<>* root = doc.first_node("window");

	windowSpecFile.close();

	// Start extracting window info

	std::string windowName;
	irect windowArea;
	int borderWidth;

	auto nameAtt = root->first_attribute("name");
	auto posAtt = root->first_attribute("pos");
	auto sizeAtt = root->first_attribute("size");
	auto borderAtt = root->first_attribute("border");
	auto movable = xmlToString(root->first_attribute("movable"));

	windowName.assign(nameAtt->value(), nameAtt->value_size());
	windowArea.topLeft = xmlToVec<glm::ivec2>(posAtt);
	windowArea.size = xmlToVec<glm::ivec2>(sizeAtt);
	borderWidth = xmlToNum<int>(borderAtt);

	UIWindow* w = new UIWindow(windowName, windowArea, borderWidth, &Engine::window);
	w->scriptsInitFunc = scripts;

	if (movable == "true")
		w->setMovable(true);
	else
		w->setMovable(false);

	// Extract all the window elements

	rapidxml::xml_node<>* elNode = root->first_node("element");

	while (elNode)
	{
		auto typeAtt = elNode->first_attribute("type");
		std::string type(typeAtt->value(), typeAtt->value_size());

		/// TODO: completeness checking for mandatory attributes

		UIElement* el = nullptr;

		std::string tab, tabSection;

		if (type == "multitab")
		{
			el = new UIMultiTab(w);
			auto tab = (UIMultiTab*)el;

			auto name = xmlToString(elNode->first_attribute("name"));
			auto pos = xmlToVec<glm::fvec2>(elNode->first_attribute("position"));
			auto size = xmlToVec<glm::fvec2>(elNode->first_attribute("size"));
			auto currentTab = xmlToString(elNode->first_attribute("currenttab"));

			tab->setName(name);
			tab->setPosition(pos);
			tab->setSize(size);

			auto tabNode = elNode->first_node("tab");
			while (tabNode)
			{
				auto tabName = xmlToString(tabNode->first_attribute("name"));
				tab->addTab(tabName);
				tabNode = tabNode->next_sibling("tab");
			}

			tab->setCurrentTab(currentTab);
		}
		else if (type == "button")
		{
			el = new UIButton(w);
			auto but = (UIButton*)el;

			auto name = xmlToString(elNode->first_attribute("name"));
			auto pos = xmlToVec<glm::fvec2>(elNode->first_attribute("position"));
			auto size = xmlToVec<glm::fvec2>(elNode->first_attribute("size"));
			auto text = xmlToString(elNode->first_attribute("text"));
			auto font = xmlToString(elNode->first_attribute("font"));
			auto fontSize = xmlToNum<int>(elNode->first_attribute("fontsize"));
			auto colour = xmlToVec<glm::fvec3>(elNode->first_attribute("fontcolour"));

			Font* fontPtr;
			if (font == "_NULL_")
				font = "clearsansb";
				
			fontPtr = Engine::assets.getFont(font);

			but->setName(name);
			but->setPosition(pos);
			but->setSize(size);
			but->getText().setFont(fontPtr);
			but->setTextColour(colour);
			but->getText().setCharSize(fontSize);
			but->setString(text);
		}
		else if (type == "label")
		{
			el = new UILabel(w);
			auto lbl = (UILabel*)el;

			auto name = xmlToString(elNode->first_attribute("name"));
			auto pos = xmlToVec<glm::fvec2>(elNode->first_attribute("position"));
			auto size = xmlToVec<glm::fvec2>(elNode->first_attribute("size"));
			auto text = xmlToString(elNode->first_attribute("text"));
			auto font = xmlToString(elNode->first_attribute("font"));
			auto fontSize = xmlToNum<int>(elNode->first_attribute("fontsize"));
			auto colour = xmlToVec<glm::fvec3>(elNode->first_attribute("fontcolour"));
			auto textOriginStr = xmlToString(elNode->first_attribute("textorigin"));

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


			lbl->setName(name);
			lbl->setPosition(pos);
			lbl->setTextOrigin(textOrigin);
			lbl->setSize(size);
			lbl->setFont(fontPtr);
			lbl->setColour(colour);
			lbl->setCharSize(fontSize);
			lbl->setString(text);
		}
		else if (type == "slider") /// TODO: better default options handling
		{
			el = new UISlider(w);
			auto sli = (UISlider*)el;

			auto name = xmlToString(elNode->first_attribute("name"));
			auto pos = xmlToVec<glm::fvec2>(elNode->first_attribute("position"));
			auto width = xmlToNum<float>(elNode->first_attribute("width"));
			auto desc = xmlToString(elNode->first_attribute("text"));
			auto font = xmlToString(elNode->first_attribute("font"));
			auto fontSize = xmlToNum<int>(elNode->first_attribute("fontsize"));
			auto colour = xmlToVec<glm::fvec3>(elNode->first_attribute("fontcolour"));
			auto valType = xmlToString(elNode->first_attribute("valuetype"));
			if (valType == "integer")
			{
				sli->valType = UISlider::Integer;
				auto limits = xmlToVec<glm::ivec2>(elNode->first_attribute("limits"));
				sli->setLimits(limits);
				auto initial = xmlToNum<int>(elNode->first_attribute("value"));
				sli->value.i = initial;
			}
			else
			{
				sli->valType = UISlider::Float;
				auto limits = xmlToVec<glm::fvec2>(elNode->first_attribute("limits"));
				sli->setLimits(limits);
				auto initial = xmlToNum<float>(elNode->first_attribute("value"));
				sli->value.f = initial;
			}

			sli->setName(name);
			sli->init(pos, width);
			sli->setDescription(desc);
		}
		else if (type == "input")
		{

		}
		else
		{

		}

		tab = xmlToString(elNode->first_attribute("tab"));
		tabSection = xmlToString(elNode->first_attribute("tabsection"));

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
