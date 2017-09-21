#pragma once
#include "File.hpp"
#include <vector>
#include "StringGenerics.hpp"
#include "Time.hpp"
#include "Engine.hpp"

class Tweaks
{
public:
	Tweaks() {}
	~Tweaks() {}

	enum Type { Integer, Floating };

	void setTweaksFile(std::string fileName)
	{
		String<128> fn;
		fn.setToChars(fileName.c_str()); ///TODO: Think about not using String<> anymore
		file.open(fn, File::Mode::in); ///TODO: There is no method for just setting file name ?
		file.close();
	}

	void setUpdateTime(Time& t)
	{
		updateTimeMS = t.getMilliSeconds();
	}

	template<class T>
	void bindVariable(T& var, std::string name, Type type)
	{
		TweakVariable tv;
		tv.type = type;
		tv.data = &var;
		tv.size = sizeof(T);
		
		vars.insert(std::make_pair(name, tv));
	}

	///TODO: Remove variable!

	void updateTweaks()
	{
		curTimeMS += Engine::dt.getMilliSeconds();
		if (curTimeMS > updateTimeMS)
		{
			updateTweaksInternal();
			curTimeMS = 0.0;
		}
	}

private:

	void updateTweaksInternal()
	{
		file.open(File::Mode::in);

		std::string tweaksName;
		file.readStr(tweaksName,'=');

		while (tweaksName.size() != 0)
		{
			std::string variable;
			file.readStr(variable, '\n');

			auto findVar = vars.find(tweaksName);

			if (findVar != vars.end())
			{
				Type type = findVar->second.type;
				void* data = findVar->second.data;
				s32 size = findVar->second.size;

				union DataBuffer {
					s8 d8;
					s16 d16;
					s32 d32;
					s64 d64;
					float flt;
					double dbl;
				} dataBuffer;

				switch (type) {
				case Integer: {

					switch (size) {
					case 1:
						dataBuffer.d8 = std::stoi(variable); break;
					case 2:
						dataBuffer.d16 = std::stoi(variable); break;
					case 4:
						dataBuffer.d32 = std::stoi(variable); break;
					case 8:
						dataBuffer.d64 = std::stol(variable); break;
					}

					memcpy(data, &dataBuffer.d64, size);
					break;
				}
				case Floating: {

					switch (size) {
					case 4:
						dataBuffer.flt = std::stof(variable); break;
					case 8:
						dataBuffer.dbl = std::stof(variable); break;
					}

					memcpy(data, &dataBuffer.dbl, size);
					break;
				}
				}
			}

			tweaksName.clear();
			file.readStr(tweaksName,'=');
		}

		file.close();
	}

	File file;
	double updateTimeMS;
	double curTimeMS;

	struct TweakVariable
	{
		Type type;
		void* data;
		s32 size;
	};


	std::map<std::string, TweakVariable> vars;

};