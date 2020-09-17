#pragma once

#include <chrono>

class SystemClock
{
public:
	static uint64_t getCurrentTimeMillis();
    static double getElapsedMillis(uint64_t timeEnd, uint64_t timeStart);
};

