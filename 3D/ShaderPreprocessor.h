#pragma once
#include "Types.h"
#include <string>
#include <vector>
#include <assert.h>
#include <map>
#include "StackStrings.h"

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
				//char varName[32];
				String32 varName;

				u32 varNameIndex = 0;
				for (;;)
				{
					varName.append(*pSource);

					++varNameIndex;

					if (varNameIndex > 31) { assert(0); }

					if (*++pSource == '%')
					{
						++pSource;
						break;
					}
				}

				//auto find = std::find(varNamesVals.begin(), varNamesVals.end(), varName);

				String1024* findVal = nullptr;

				{
					for (u32 i = 0; i < 32; ++i)
					{
						//if (memcmp(varName, varVals[i].var, 32) == 0)
						if (varName == varVals[i].var)
						{
							findVal = &varVals[i].val;
							break;
						}
					}
				}

				if (findVal == nullptr)
					assert(0);

				StackString::copyChars(&processedShader[outputIndex], findVal->getString(), findVal->getLength());
				outputIndex += findVal->getLength();
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

	void setVarVal(String32& var, String1024& val)
	{
		for (u32 i = 0; i < 32; ++i)
		{
			if (varVals[i].var == var)
			{
				varVals[i].val.overwrite(val);
				break;
			}

			if (varVals[i].var.getString()[0] == '\0')
			{
				varVals[i].val.overwrite(val);
				varVals[i].var.overwrite(var);
				break;
			}
		}
	}

private:

	struct VarAndVal
	{
		String32 var;
		String1024 val;
	};

	VarAndVal varVals[32];
};