#pragma once

#include <thread>
#include <opencv2/opencv.hpp>
#include "Utils.h"

class SoundPressureSensor;

class RecordThread
{
protected:
	std::thread *recordThread;
    SoundPressureSensor* sensor = NULL;
    uint64_t lTriggerTime = 0;
    TargetCircle findCircle(cv::Mat frame);
    cv::Ptr<cv::SimpleBlobDetector> detector;
public:
    RecordThread(float TRIGGER_DB, int mic_index);
	virtual void start();
	virtual void stop();
	virtual bool isRunning();
	void join();
    void audio_trigger(uint64_t trigger_time);
private:
    void run();
};

