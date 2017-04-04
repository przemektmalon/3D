#pragma once
#include "StackStrings.h"
#include <iostream>
#include <io.h>
#include <fstream>
#include <time.h>

class Logger
{
public:
	Logger() {}
	~Logger() {}

	void printLog(char* log, u32 length, StackString& logName)
	{
		logName.insertAt(0, String32("logs/"));
		std::ofstream ofs(logName.getString(), std::ios_base::binary);
		ofs.write(log, length);
		ofs.close();
	}

	void printLog(char* log, u32 length)
	{
		time_t now = time(0);
		tm  tstruct;
		String128 logName;
		char buf[80];
		localtime_s(&tstruct, &now);
		strftime(buf, sizeof(buf), "%F_%H;%M;%S", &tstruct);
		printLog(log, length, String128(buf).append(String32(".txt")));
	}
};