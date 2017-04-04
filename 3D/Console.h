#pragma once
#include <string>
#include "Types.h"
#include "Text.h"

class Console
{
public:
	Console() {}
	~Console() {}

	void commandEnter(std::string pCommand)
	{


	}



private:

	//float* getMatrix()

	//

	//Func Reg CALL command
	//Params { NUM_PARAMS, PARAM_ARRAY (Param types), FUNC_POINTER }
	//
	//
	//

	class Command
	{
	public:
		Command() {}
		~Command() {}

		enum CommandType { Call, Set, Get };
	};

	class CType
	{
		enum Type { Int, Float, String, Vector, List, Array };
		Type type;
	};

	class CallCommand : public Command
	{
	public:

		struct CallParameter
		{
			enum Type { Int, Float, String };
			template<typename T>
			CallParameter(Type pType, T pValue) { type = pType; value = reinterpret_cast<u64>(pValue); }
			Type type;
			u64 value;
		};

		//u32 NUM_PARAMS , Parameter* PARAMS
		u32 numParams;
		CallParameter* params;


		void execute()
		{

		}
	};

	class GetCommand : public Command
	{
	public:

		struct GetReturn
		{

			template<typename T>
			GetReturn(CType pType, T pValue, u64 pArraySize = 0) { type = pType; value = reinterpret_cast<u64>(pValue); arraySize = pArraySize; }
			CType type;
			u64 value;
			u64 arraySize;
		};
	};

	Command* interpretCommand()
	{

	}

	void print(std::string pString)
	{
		if (nextLine == maxLines)
		{
			moveLines(1);
		}
		lines[nextLine].setString(pString);
	}

	void moveLines(u32 pNumLines)
	{
		if (pNumLines == 0)
		{
			return;
		}

		if (lastLine - pNumLines < 0)
		{
			//CLEAR ALL LINES
		}

		for (int i = lastLine - pNumLines; i >= 0; --i)
		{
			lines[i].setString(lines[i + pNumLines].getString());
		}
	}

	u32 lastLine;
	u32 nextLine;

	static const u32 maxLines = 32;
	Text lines[maxLines];

};