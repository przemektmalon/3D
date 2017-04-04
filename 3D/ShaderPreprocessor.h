#pragma once
#include "Types.h"
#include <string>
#include <vector>
#include <assert.h>
#include <map>

class ShaderPreprocessor
{
public:
	ShaderPreprocessor() {}
	~ShaderPreprocessor() {}

	char* preProcess(char* pSource, s32 size)
	{
		char* processedShader = new char[size + (1024 * 1024)];

		bool converting = false;

		int sourceIndex = 0;
		int outputIndex = 0;

		for (;;)
		{
			if (*pSource == '%')
				converting = true;

			if (converting)
			{
				++pSource;
				//std::string varName; varName.reserve(20);
				char varName[32];
				u32 varNameIndex = 0;
				for (;;)
				{
					varName[varNameIndex] = *pSource;
					++varNameIndex;

					if (varNameIndex > 31) { assert(0); }

					if (*++pSource == '%')
					{
						++pSource;
						break;
					}
				}

				//auto find = std::find(varNamesVals.begin(), varNamesVals.end(), varName);

				char* findVal = nullptr;


				{
					for (u32 i = 0; i < 32; ++i)
					{
						if (memcmp(varName, varVals[i].var, 32) == 0)
						{
							findVal = varVals[i].val;
							break;
						}
					}
				}


				if (findVal == nullptr)
					assert(0);

				u32 valIndex = 0;
				for (;;)
				{
					if (findVal[valIndex] != '\0')
					{
						++valIndex;
						processedShader[outputIndex] = findVal[valIndex];
						++outputIndex;
					}
					else
					{
						break;
					}
				}
				converting = false;
			}

			processedShader[outputIndex] = *pSource;

			++outputIndex;
			++sourceIndex;

			if (*++pSource == '\0')
				break;
		}
		processedShader[outputIndex] = '\0';
		return processedShader;
	}

	void setVarVal(char* var, char* val)
	{
		for (u32 i = 0; i < 32; ++i)
		{
			if (memcmp(var, varVals[i].var, 32))
			{
				memcpy(varVals[i].val, val, 1024 - 32);
			}

			if (varVals[i].var[0] == '\0')
			{
				memcpy(varVals[i].var, var, 32);
				memcpy(varVals[i].val, val, 1024 - 32);
			}
		}
	}

private:

	struct VarAndVal
	{
		char var[32];
		char val[1024 - 32];
	};

	//std::map<std::string,std::string> varNamesVals;
	VarAndVal varVals[32];


};