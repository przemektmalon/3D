#pragma once
#include "Types.hpp"

class Time
{
public:
	Time() : seconds(0), milliSeconds(0), microSeconds(0) {};
	Time(double pSeconds) { setSeconds(pSeconds); }
	~Time() {};

	Time& setMicroSeconds(s64 ms) { microSeconds = ms; milliSeconds = double(ms) * 0.001; seconds = double(ms) * 0.000001; return *this; }
	Time& setMilliSeconds(double ms) { milliSeconds = ms; microSeconds = ms * 1000; seconds = ms * 0.001; return *this; }
	Time& setSeconds(double s) { seconds = s; microSeconds = s * 1000000; milliSeconds = s * 1000; return *this; }

	s64 getMicroSeconds() { return microSeconds; }
	double getMilliSeconds() { return milliSeconds; }
	float getMilliSecondsf() { return float(milliSeconds); }
	double getSeconds() { return seconds; }
	float getSecondsf() { return float(seconds); }

	s64 removeMicroSeconds(s64 ms) { setMicroSeconds(microSeconds - ms); return microSeconds; }
	double removeMilliSeconds(double ms) { setMilliSeconds(milliSeconds - ms); return milliSeconds; }
	double removeSeconds(double s) { setSeconds(seconds - s); return seconds; }

	s64 addMicroSeconds(s64 ms) { setMicroSeconds(microSeconds + ms); return microSeconds; }
	double addMilliSeconds(double ms) { setMilliSeconds(milliSeconds + ms); return milliSeconds; }
	double addSeconds(double s) { setSeconds(seconds + s); return seconds; }

	void operator-(Time& rhs)
	{
		microSeconds -= rhs.getMicroSeconds();
		milliSeconds -= rhs.getMilliSeconds();
		seconds -= rhs.getSeconds();
	}

	void operator+(Time& rhs)
	{
		microSeconds += rhs.getMicroSeconds();
		milliSeconds += rhs.getMilliSeconds();
		seconds += rhs.getSeconds();
	}

	void operator=(Time& rhs)
	{
		setMicroSeconds(rhs.getMicroSeconds());
	}

private:

	s64 microSeconds;
	double milliSeconds;
	double seconds;
};