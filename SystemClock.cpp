#include "SystemClock.h"

uint64_t SystemClock::getCurrentTimeMillis() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

double SystemClock::getElapsedMillis(uint64_t timeEnd, uint64_t timeStart)
{
    return static_cast<double> (timeEnd - timeStart);
}
