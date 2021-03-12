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
	int sn = 0;
	bool stopRecording = false;
    double RATIO1; // px from camera to mm = 0.846 for clamped test
    Vector2D adjustmentVec; // = {642.381, 371.826} for clamped test
    Vector2D fineAdjustment; // = {0, 0} for clamped test
    bool upDownDetection;
    int delay_read = 0;
    int idle_fps = 30;
public:
    ShootThread(int startSn, cv::VideoCapture video, int mic_index, bool upDownDetection, float TRIGGER_DB, int minThreshold, int maxThreshold, double RATIO1, Vector2D adjustmentVec, Vector2D fineAdjustment, ShootController page, FILE* logFile);
    void audio_trigger(uint64_t trigger_time);
	void start();
	void stop();
	bool isRunning();

private:
	FILE* logFile;
	void run();
};
