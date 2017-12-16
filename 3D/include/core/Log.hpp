#pragma once
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
	Log(std::string& pName) : timeStamp(true)
	{
		name = pName;
	}
	~Log() {}

	void clearLog()
	{
		content.clear();
	}

	static void printLog(char* log, u32 length, std::string logName)
	{
		logName.insert(0, "logs/");
		std::ofstream ofs(logName.c_str());
		ofs.write(log, length);
		ofs.close();
	}

	static void printLog(char* log, u32 length)
	{
		std::string logName;
		getDateTimeStr(logName);
		printLog(log, length, logName.append(".log"));
	}

	static void printLog(Log& log, std::string logName)
	{
		if (log.getLength() > 0)
		{
			printLog(log.getContent(), log.getLength(), logName.append(".log"));
		}
	}

	static void printLog(Log& log)
	{
		std::string logName;
		getDateTimeStr(logName);
		printLog(log, logName);
	}

	void printLog()
	{
		printLog(*this);
	}

	void postMessage(std::string& str, s32 printTimeStamp = -1)
	{
		postMessage(str.c_str(), printTimeStamp);
	}

	void postMessage(const char* str, s32 printTimeStamp = -1)
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

	void setName(std::string& pName)
	{
		name = pName;
	}

	char* getContent() { return &content[0]; }
	int getLength() { return content.size(); }

private:
	std::string name;

	std::string content;

	bool timeStamp;

};
