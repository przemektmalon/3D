#include "Keyboard.hpp"
#include "Engine.hpp"

char Keyboard::convertToASCII(Key key)
{
	char c = MapVirtualKey(key.code, MAPVK_VK_TO_CHAR);
	if (key.isPrintable(c))
	{
		if (key.isAlpha(c))
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
	}
	return c;
}