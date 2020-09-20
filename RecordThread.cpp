#include "RecordThread.h"

using namespace std;

void RecordThread::start()
{
    recordThread = new thread(&RecordThread::run, this);
}

void RecordThread::stop()
{
}

bool RecordThread::isRunning()
{
    return recordThread->joinable();
}

void RecordThread::run()
{
}

void RecordThread::join()
{
	recordThread->join();
}
