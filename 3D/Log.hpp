#pragma once
#include "Strings.hpp"
#include "Types.hpp"
#include <iostream>
#include <io.h>
#include <fstream>
#include <time.h>
#include "Util.hpp"

class Log
{
public:
	Log() : timeStamp(true)
	{
		
	}
	Log(String<128>& pName) : timeStamp(true)
	{
		name.overwrite(pName);
	}
	~Log() {}

	void clearLog()
	{
		content.clear();
	}

	static void printLog(char* log, u32 length, StringGeneric& logName)
	{
		logName.insertAt(0, String32("logs/"));
		std::ofstream ofs(logName.getString());
		ofs.write(log, length);
		ofs.close();
	}

	static void printLog(char* log, u32 length)
	{
		String<128> logName;
		getDateTimeStr(logName);
		printLog(log, length, logName.append(String32(".log")));
	}

	static void printLog(Log& log, StringGeneric& logName)
	{
		if (log.getLength() > 0)
		{
			printLog(log.getContent(), log.getLength(), logName.append(String<32>(".log")));
		}
	}

	static void printLog(Log& log)
	{
		String<128> logName;
		getDateTimeStr(logName);
		printLog(log, logName);
	}

	void printLog()
	{
		printLog(*this);
	}

	void postMessage(StringGeneric& str, s32 printTimeStamp = -1)
	{
		postMessage(str.getString(), printTimeStamp);
	}

	void postMessage(char* str, s32 printTimeStamp = -1)
	{
		if (timeStamp && printTimeStamp != 0)
		{
			postTime();
			content.append(str);
			content.append("\n");
		}
		else if (!timeStamp && printTimeStamp != 1)
		{
			content.append(str);
			content.append("\n");
		}
	}

	void postTime()
	{
		std::string time;
		time.append("@");
		getTimeStr(time, ':');
		time.append(" - ");
		content.append(time);
	}

	void appendMessage(const char* str)
	{
		content.append(str);
	}

	void setName(String<128>& pName)
	{
		name.overwrite(pName);
	}

	char* getContent() { return &content[0]; }
	int getLength() { return content.size(); }

private:
	//String<HEAP> content;
	String<128> name;

	std::string content;

	bool timeStamp;

};
