#include "Console.h"

std::map<String32, u32> Console::funcIDs;

void test(int x) {
	std::cout << "X:" << x << "\n";
	std::string s; s = std::to_string(x);
	OutputDebugStringA(s.c_str());
}

void test2(int x, float y) {
	OutputDebugStringA((std::to_string(x) + std::to_string(y)).c_str());
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
	OutputDebugStringA(string.c_str());
}

void Console::registerConsoleFuncs()
{
	REGISTER_CONSOLE_CALLABLE(3, "FUNCNAME", 6);
	REGISTER_CONSOLE_CALLABLE(2, "FOO", 2);
	REGISTER_CONSOLE_CALLABLE(1, "FUNC", 1);
}

void Console::submitCommand(String512& command)
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
			assert(0);
		else
		{
			for (;;)
			{
				if (*cc == ' ')
					break;

				if (!isAlpha(*cc))
					assert(0);
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

	BEGIN_FUNC_SWITCH(funcName)
		CHECK_CONSOLE_CALLABLE(1, test, 1);
		CHECK_CONSOLE_CALLABLE(2, test2, 2);
		CHECK_CONSOLE_CALLABLE(3, test3, 6);
	END_FUNC_SWITCH

}