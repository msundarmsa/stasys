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
	double RATIO1; // px from camera to cm
	double radius; // radius of black circle (px)
	Vector2D adjustmentVec;
    ShootController page;
	bool audio_triggered = false;
	int sn = 0;
	cv::SimpleBlobDetector::Params params;
	cv::Ptr<cv::SimpleBlobDetector> detector;
	SoundPressureSensor* sensor;
	bool stopRecording = false;
public:
    ShootThread(cv::VideoCapture video, double radius, Vector2D adjustmentVec, ShootController page, FILE* logFile);
	void audio_trigger();
	void start();
	void stop();
	bool isRunning();

private:
	TargetCircle findCircle(cv::Mat frame);
	FILE* logFile;
	void run();
};
