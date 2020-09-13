#pragma once

#include "Utils.h"
#include "Shot.h"
#include <opencv2/opencv.hpp>
#include "SystemClock.h"
#include "RecordThread.h"

class SoundPressureSensor;

class ShootThread : public RecordThread {
private:
	cv::VideoCapture video;
    ShootController page;
	bool audio_triggered = false;
	int sn = 0;
	cv::SimpleBlobDetector::Params params;
	cv::Ptr<cv::SimpleBlobDetector> detector;
    SoundPressureSensor* sensor = NULL;
	bool stopRecording = false;
    float TRIGGER_DB;
    double RATIO1; // px from camera to mm
    Vector2D adjustmentVec;
    Vector2D fineAdjustment;
    bool upDownDetection;
public:
    ShootThread(int startSn, cv::VideoCapture video, std::string mic, bool upDownDetection, float TRIGGER_DB, double RATIO1, Vector2D adjustmentVec, Vector2D fineAdjustment, ShootController page, FILE* logFile);
	void audio_trigger();
	void start();
	void stop();
	bool isRunning();

private:
	TargetCircle findCircle(cv::Mat frame);
	FILE* logFile;
	void run();
};
