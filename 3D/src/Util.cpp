#include "Strings.hpp"
#include <time.h>

void getDateTimeStr(StringGeneric& str)
{
	time_t now = time(0);
	tm  tstruct;
	String128 logName;
	char buf[80];
	localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), "%F %H,%M,%S", &tstruct);
	str.setToChars(buf);
}

void getTimeStr(StringGeneric& str, char delim = ';')
{
	time_t now = time(0);
	tm  tstruct;
	String128 logName;
	char buf[80];
	localtime_s(&tstruct, &now);

	String<9> formatStr;
	formatStr.append("%H",2);
	formatStr.append(delim);
	formatStr.append("%M", 2);
	formatStr.append(delim);
	formatStr.append("%S", 2);
	//strftime(buf, sizeof(buf), "%H;%M;%S", &tstruct);
	strftime(buf, sizeof(buf), formatStr.getString(), &tstruct);
	str.setToChars(buf);
}

void getTimeStr(std::string& str, char delim = ';')
{
	time_t now = time(0);
	tm  tstruct;
	String128 logName;
	char buf[80];
	localtime_s(&tstruct, &now);

	char fs[9]; // Format string
	fs[0] = '%';
	fs[1] = 'H';
	fs[2] = delim;
	fs[3] = '%';
	fs[4] = 'M';
	fs[5] = delim;
	fs[6] = '%';
	fs[7] = 'S';
	fs[8] = '\0';
	strftime(buf, sizeof(buf), fs, &tstruct);
	str = buf;
}