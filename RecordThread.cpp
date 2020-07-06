#include "RecordThread.h"

void RecordThread::start()
{
    recordThread = new std::thread(&RecordThread::run, this);
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
