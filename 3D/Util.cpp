#include "Strings.h"
#include <time.h>

void getDateTimeStr(StringGeneric& str)
{
	time_t now = time(0);
	tm  tstruct;
	String128 logName;
	char buf[80];
	localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), "%F_%H;%M;%S", &tstruct);
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