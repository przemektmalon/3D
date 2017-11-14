#pragma once
#include "Console.hpp"
#include "AssetManager.hpp"
#include "World.hpp"

void listShaders()
{
	auto& shaderMap = Engine::r->shaderStore.getShaderMap();
	for (auto itr = shaderMap.begin(); itr != shaderMap.end(); ++itr)
	{
		Engine::console.postResult(const_cast<String32&>(itr->first));
	}
}

void reloadShader(String32 shaderName)
{
	Engine::r->shaderStore.reloadShader(shaderName);
}

void reloadAllShaders()
{
	Engine::r->shaderStore.reloadAllShaders();
}

//MODEL LOADING AND MANIPULATION

std::map<String32, u32> Console::funcIDs;

void Console::submitCommand(StringGeneric& command)
{
	//INTERPRET COMMAND
	//IGNORE LEADING SPACES
	//GET FUNC NAME UNTIL NEXT SPACE (ONLY ALPHA CHARS)
	//IGNORE SPACES
	//INTERPRET PARAMS
	//IF NUM
	//GET NUM OR FLOAT
	//IF '"'
	//GET STRING UNTIL NEXT '"'
	//IF '('
	//INTERPRET FOLLOWING TYPES (MAYBE ANOTHER VECTOR)
	//WHEN ')' REACHED 
	//IF '['
	//GET FOLLOWING PARAMS (SAME TYPES)

	String32 funcName;
	char* cc = command.getString();

	//CNumber numParam;
	//CVector vecParam;

	std::vector<CParam> params; params.reserve(4);

	bool noParams = false;

	for (;;)
	{
		if (*cc == ' ')
		{
			++cc; continue;
		}

		if (!isAlpha(*cc))
			//assert(0);
			return; ///TODO: engineLog ?
		else
		{
			for (;;)
			{
				if (*cc == ' ')
					break;

				if (*cc == '\0') //Command with no parameters
				{
					noParams = true; break;
				}

				if (!isAlpha(*cc))
					return;
				else
					funcName.append(*cc); ++cc;
			}
			break;
		}
		++cc;
	}

	if (!noParams)
	{
		for (;;)
		{
			if (*++cc == '\0')
				break;

			if (*cc == ' ')
				continue;

			if (*cc == '-') //NUMBER (INT OR FLOAT)
			{
				if (*++cc == '(')
					params.push_back(new CVector(interpretVector(&cc)));
				else
					params.push_back(new CNumber(interpretNumber(&cc)));
				continue;
			}

			if (isNum(*cc)) //NUMBER (INT OR FLOAT)
			{
				params.push_back(new CNumber(interpretNumber(&cc)));
				continue;
			}

			if (*cc == '"') //STRING
			{
				params.push_back(new String32(interpretString(&cc)));
				++cc;
				continue;
			}

			if (*cc == '(') //VECTOR
			{
				params.push_back(new CVector(interpretVector(&cc)));
				continue;
			}
		}
	}

	BEGIN_FUNC_SWITCH(funcName)
		CHECK_CONSOLE_CALLABLE(2, listShaders, 0);
		CHECK_CONSOLE_CALLABLE(3, reloadShader, 1);
		CHECK_CONSOLE_CALLABLE(4, reloadAllShaders, 0);
	END_FUNC_SWITCH
}

void Console::registerConsoleFuncs()
{
	REGISTER_CONSOLE_CALLABLE(2, "listShaders", 0);
	REGISTER_CONSOLE_CALLABLE(3, "reloadShader", 1);
	REGISTER_CONSOLE_CALLABLE(4, "reloadAllShaders", 0);
}

void Console::textInput(Key code)
{
	
	switch (code.code)
	{
	case VK_LEFT:
	{
		cursor = cursor > 0 ? cursor - 1 : cursor;
		break;
	}
	case VK_RIGHT:
	{
		cursor = cursor + 2 < cmd.getString().getLength() ? cursor + 1 : cursor;
		break;
	}
	case VK_BACK: //Backspace
	{
		if (cursor == 0)
			break;
		--cursor;
		cmd.getString().shrinkBy(1);
		cmd.forceUpdate();
		break;
	}
	case VK_DELETE: //Delete
	{
		if (cursor + 2 == cmd.getString().getLength())
			break;
		cmd.getString().removeAt(cursor+2, 1);
		cmd.forceUpdate();
		break;
	}
	case VK_ESCAPE: //Escape
	{
		stateFlags = 0;
		break;
	}
	case VK_OEM_3:  //Tilde
	{
		if (!Engine::window.keyboard.isKeyPressed(VK_SHIFT))
		{
			stateFlags = 0;
			break;
		}
		else
		{
			char c = '~';
			cmd.getString().append(c);
			cmd.forceUpdate();
			++cursor;
			break;
		}
	}
	case VK_RETURN: //Enter / carriage return
	{
		String<HEAP> submit;
		submit.overwrite(cmd.getString());
		submit.removeAt(0, 2);
		consoleHistory.push_back(new Text2D());
		consoleHistory.back()->init();
		consoleHistory.back()->setFont(Engine::assets.getFont(String32("consola")));
		consoleHistory.back()->setCharSize(25);
		consoleHistory.back()->setString(cmd.getString());
		submitCommand(submit);
		consoleHistory.back()->setTextOrigin(Text2D::Origin::TopLeft);
		cmd.setString(String<3>("> "));
		//cmd.forceUpdate();
		cursor = 0;
		repositionText();
		break;
	}
	default:
	{
		char c = MapVirtualKey(code.code, MAPVK_VK_TO_CHAR);
		if (isPrintable(c))
		{
			if (isAlpha(c))
			{
				if (!Engine::window.keyboard.isKeyPressed(VK_SHIFT))
					c += 32;
			}
			else
			{
				if (Engine::window.keyboard.isKeyPressed(VK_SHIFT))
				{
					switch (c)
					{
					case '1':
						c = '!';
						break;
					case '2':
						c = '@';
						break;
					case '3':
						c = '#';
						break;
					case '4':
						c = '$';
						break;
					case '5':
						c = '%';
						break;
					case '6':
						c = '^';
						break;
					case '7':
						c = '&';
						break;
					case '8':
						c = '*';
						break;
					case '9':
						c = '(';
						break;
					case '0':
						c = ')';
						break;
					case '-':
						c = '_';
						break;
					case '=':
						c = '+';
						break;
					case '[':
						c = '{';
						break;
					case ']':
						c = '}';
						break;
					case ';':
						c = ':';
						break;
					case '\'':
						c = '"';
						break;
					case '\\':
						c = '|';
						break;
					case ',':
						c = '<';
						break;
					case '.':
						c = '>';
						break;
					case '/':
						c = '?';
						break;
					case '`':
						c = '~';
						break;
					}
				}
			}

			cmd.getString().append(c);
			cmd.forceUpdate();
			++cursor;
		}
		else
		{	
			//cmd.getString().append('~');
		}
		break;
	}
	}
}

void Console::init()
{
	cmd.init();
	cmd.setFont(Engine::assets.getFont(String32("consola")));
	cmd.setCharSize(25);
	cmd.setString(String<3>("> "));
	cmd.setColour(glm::fvec3(1.f, 0.f, 0.f));
	cmd.setTextOrigin(Text2D::Origin::TopLeft);
	repositionText();
}