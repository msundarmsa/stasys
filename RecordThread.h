#pragma once

#include <thread>

class RecordThread
{
protected:
	std::thread *recordThread;
public:
	virtual void start();
	virtual void stop();
	virtual bool isRunning();
	void join();
private:
    void run();
};

