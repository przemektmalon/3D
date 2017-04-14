#pragma once
#include "Strings.h"
#include "Types.h"
#include <iostream>
#include <io.h>
#include <fstream>
#include <time.h>
#include "Util.h"

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

	void postMessage(StringGeneric& str)
	{
		if (timeStamp)
		{
			String<50> time; 
			time.append("@",1);
			getTimeStr(time,':');
			time.append(" - ",3);
			content.append(time);
			content.append(str);
			content.append("\n",1);
		}
		else
		{

		}
	}

	void postMessage(char* str)
	{
		if (timeStamp)
		{
			String<50> time;
			time.append("@", 1);
			getTimeStr(time, ':');
			time.append(" - ", 3);
			content.append(time);
			content.append(str, String<0>::determineLength(str));
			content.append("\n", 1);
		}
		else
		{

		}
	}

	void setName(String<128>& pName)
	{
		name.overwrite(pName);
	}

	char* getContent() { return content.getString(); }
	int getLength() { return content.getLength(); }

private:
	String<HEAP> content;
	String<128> name;

	bool timeStamp;

};

class Logger
{
public:
	Logger() {}
	~Logger() {}

	void printLog(char* log, u32 length, StringGeneric& logName)
	{
		logName.insertAt(0, String32("logs/"));
		std::ofstream ofs(logName.getString(), std::ios_base::binary);
		ofs.write(log, length);
		ofs.close();
	}

	void printLog(char* log, u32 length)
	{
		String<128> logName;
		getDateTimeStr(logName);
		printLog(log, length, logName.append(String32(".log")));
	}

	void printLog(Log& log, StringGeneric& logName)
	{
		if (log.getLength() > 0)
		{
			printLog(log.getContent(), log.getLength(), logName.append(String<32>(".log")));
		}
	}

	void printLog(Log& log)
	{
		String<128> logName;
		getDateTimeStr(logName);
		printLog(log, logName);
	}

private:
	String<HEAP> engineLog;

};