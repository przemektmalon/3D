#pragma once
#include <Windows.h>

#include <chrono>

class QPC
{
public:
	QPC() {}
	~QPC() {}

	u64 now()
	{
		auto t = std::chrono::time_point_cast<std::chrono::microseconds>(clock.now());
		auto t2 = t.time_since_epoch();
		return t2.count();
	}

private:

	std::chrono::high_resolution_clock clock;
};

