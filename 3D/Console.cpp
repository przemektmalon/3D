#include "Console.h"
#include "AssetManager.h"
#include "World.h"

std::map<String32, u32> Console::funcIDs;

void test(int x) {
	std::cout << "X:" << x << "\n";
	std::string s; s = std::to_string(x);
	Engine::console.postMessage(String<HEAP>(s.c_str()));
}

void test2(int x, float y) {
	std::string string = (std::to_string(x) + std::to_string(y));
	Engine::console.postMessage(String<HEAP>(string.c_str()));
}

void test3(glm::ivec3 iv3, glm::fvec2 fv2, int i, String32 str, float f, int i2)
{
	std::stringstream ss;
	ss << "(" << iv3.x << ", " << iv3.y << ", " << iv3.z << ")" << std::endl;
	ss << "(" << fv2.x << ", " << fv2.y << ")" << std::endl;
	ss << i << std::endl;
	ss << str.getString() << std::endl;
	ss << f << std::endl;
	ss << i2;
	std::string string;
	string = ss.str();
	Engine::console.postMessage(String<HEAP>(string.c_str()));
}

void setModelPosition(glm::fvec3 pos)
{
	if (Engine::selectedID == -1) return;
	auto i = Engine::world->getMeshInstance(Engine::selectedID);
	i->sgNode->transform.setTranslation(pos);
	i->sgNode->updateAll();
	Engine::world->updateGLBuffers();
}

void Console::registerConsoleFuncs()
{
	REGISTER_CONSOLE_CALLABLE(3, "FUNCNAME", 6);
	REGISTER_CONSOLE_CALLABLE(2, "FOO", 2);
	REGISTER_CONSOLE_CALLABLE(1, "FUNC", 1);
	REGISTER_CONSOLE_CALLABLE(4, "SetModelPos", 1);
}

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

	for (;;)
	{
		if (*cc == ' ')
		{
			++cc; continue;
		}

		if (!isAlpha(*cc))
			//assert(0);
			return; ///TODO: log ?
		else
		{
			for (;;)
			{
				if (*cc == ' ')
					break;

				if (!isAlpha(*cc))
					//assert(0);
					return; ///TODO: log ?
				else
					funcName.append(*cc); ++cc;
			}
			break;
		}
		++cc;
	}

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

	if (params.size() == 0)
	{
		postMessage(String32("SOMETHING WENT WRONG!"));
		return;
	}

	try {
		BEGIN_FUNC_SWITCH(funcName)
			CHECK_CONSOLE_CALLABLE(1, test, 1);
			CHECK_CONSOLE_CALLABLE(2, test2, 2);
			CHECK_CONSOLE_CALLABLE(3, test3, 6);
			CHECK_CONSOLE_CALLABLE(4, setModelPosition, 1);
		END_FUNC_SWITCH
	}
	catch (...)
	{
		postMessage(String32("SOMETHING WENT WRONG!"));
	}

}

void Console::textInput(KeyCode code)
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
		consoleHistory.back()->setFont(Engine::assets.getFont(String32("clearsans")));
		consoleHistory.back()->setCharSize(25);
		consoleHistory.back()->setString(cmd.getString());
		submitCommand(submit);
		//cmdHistory.back().setTextOrigin(Text2D::Origin::TopLeft);
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
	cmd.setFont(Engine::assets.getFont(String32("clearsans")));
	cmd.setCharSize(25);
	cmd.setString(String<3>("> "));
	//cmd.setTextOrigin(Text2D::Origin::TopLeft);
	repositionText();
}