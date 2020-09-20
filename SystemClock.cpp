#include "SystemClock.h"

using namespace std::chrono;

uint64_t SystemClock::getCurrentTimeMillis() {
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

double SystemClock::getElapsedMillis(uint64_t timeEnd, uint64_t timeStart)
{
    return static_cast<double> (timeEnd - timeStart);
}
