#include <string>
#include <time.h>

void getDateTimeStr(std::string& str)
{
	time_t now = time(0);
	tm  tstruct;
	std::string logName;
	char buf[80];
	localtime_s(&tstruct, &now);
	strftime(buf, sizeof(buf), "%F %H,%M,%S", &tstruct);
	str = buf;
}


void getTimeStr(std::string& str, char delim = ';')
{
	time_t now = time(0);
	tm  tstruct;
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