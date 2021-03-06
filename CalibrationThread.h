#pragma once

#include <opencv2/opencv.hpp>
#include "SystemClock.h"
#include "RecordThread.h"
#include "Utils.h"
#include "SoundPressureSensor.cpp"
#include "ShotTrace.h"
#include <numeric>

using namespace std;
using namespace cv;

class CalibrationThread : public RecordThread {
	private:
        VideoCapture video;
        vector<TraceCircle> currentTrace;
        function<void(bool, double, double, double)> calibrationFinished;
		bool stopRecording = false;
		FILE *logFile;
	public:
        CalibrationThread(VideoCapture video, int mic_index, float TRIGGER_DB, function<void(bool, double, double, double)> calibrationFinished, FILE* logFile) : RecordThread(TRIGGER_DB, mic_index) {
			this->video = video;
            this->calibrationFinished = calibrationFinished;
			this->logFile = logFile;
		}

		void stop()
		{
			if (!stopRecording) {
				stopRecording = true;
				if (recordThread->joinable()) {
					recordThread->join();
				}
			}
		}

		void start() {
            recordThread = new thread(&CalibrationThread::run, this);
		}

		bool isRunning()
		{
			return !stopRecording;
		}

	private:
        void run() {
            Mat frame;

            ShotTrace currShotTrace;

            uint64_t lStartTime = 0;
            uint64_t lFrameTime = 0;
            uint64_t lShotStartTime = 0;
            uint64_t lLastCircleDetectedFrameTime = 0;

            int frameid = 0;

            bool shotStarted = false;

            if (sensor != NULL) {
                sensor->start();
            }

            #ifdef QT_QML_DEBUG
                int testTrigger = 312;
            #endif

            while (!stopRecording) {
                video >> frame;
                lFrameTime = SystemClock::getCurrentTimeMillis();

                if (frame.empty()) {
                    // if camera becomes disconnected, exit
                    fprintf(logFile, "\tDropped frame!\n");
                    break;
                }

                #ifdef QT_QML_DEBUG
                    fprintf(logFile, "C\t%" PRIu64 "", lFrameTime);
                #else
                    fprintf(logFile, "C,%" PRIu64 "", lFrameTime);
                #endif

                if (frameid == 0) {
                    lStartTime = lFrameTime;
                }

                #ifdef QT_QML_DEBUG
                    if (sensor == NULL && frameid == testTrigger) {
                        lTriggerTime = lFrameTime;
                    }
                #endif

                double totalMillis = SystemClock::getElapsedMillis(lFrameTime, lStartTime);
                if (totalMillis > 30000) {
                    // if it has been more than 30s, calibration has failed
                    calibrationFinished(false, -1, -1, -1);
                }

                double millisSinceShotStart = SystemClock::getElapsedMillis(lFrameTime, lShotStartTime);

                if (shotStarted) {
                    // shot has started i.e. the aim has went past the top edge and came back down
                    double millisSinceCircleDetected = SystemClock::getElapsedMillis(lFrameTime, lLastCircleDetectedFrameTime);
                    if (millisSinceCircleDetected > 2000) {
                        // reset shot if shot has started but aim is not within the target/cannot be found
                        // for 2s
                        shotStarted = false;
                        currShotTrace.reset();
                    } else if (millisSinceShotStart > 60000 && !currShotTrace.isShotPointSet()) {
                        // reset trace if shot has started but trigger has not been pulled for 60s
                        currShotTrace.reset();

                        // but update the start time to the current time
                        lShotStartTime = lFrameTime;
                    }
                }

                TargetCircle circle = findCircle(frame);
                #ifdef QT_QML_DEBUG
                    fprintf(logFile, "\t[%s]", (shotStarted ? "true" : "false"));
                #else
                    fprintf(logFile, ",%d", shotStarted);
                #endif
                if (circle.radius != -1) {
                    // aim i.e. black circle was found
                    lLastCircleDetectedFrameTime = lFrameTime;
                    Vector2D center = { circle.center.x, circle.center.y };
                    #ifdef QT_QML_DEBUG
                        fprintf(logFile, "\t(%.3f , %.3f)", center.x, center.y);
                    #else
                        fprintf(logFile, ",%.3f,%.3f", center.x, center.y);
                    #endif

                    if (center.x >= -TARGET_SIZE / 2 && center.x <= TARGET_SIZE / 2 &&
                        center.y >= -TARGET_SIZE / 2 && center.y <= TARGET_SIZE / 2) {
                        // aim is found and within the target
                        lLastCircleDetectedFrameTime = lFrameTime;
                    }

                    if (!shotStarted) {
                        // new shot started
                        shotStarted = true;

                        // reset traces
                        currShotTrace.reset();

                        lShotStartTime = lFrameTime;
                    } else {
                        if (lTriggerTime != 0) {
                            // trigger has just been pulled
                            #ifdef QT_QML_DEBUG
                                fprintf(logFile, "\tTRIGGER RECEIVED AT %llu", lTriggerTime);
                            #else
                                fprintf(logFile, ",%llu", lTriggerTime);
                            #endif

                            vector<TracePoint> beforeShotTrace = currShotTrace.getBeforeShotTrace();
                            vector<TracePoint> points;
                            TargetCircle bestAvgCircle = {{0, 0}, -1};
                            double bestVariance = -1;

                            for (size_t i = 0; i < beforeShotTrace.size(); i++) {
                                TracePoint currTP = beforeShotTrace[i];
                                if (points.size() == 0) {
                                    points.push_back(currTP);
                                } else {
                                    double duration = currTP.time - points[0].time;
                                    if ((duration > 1000 && duration < 1050) || (i == beforeShotTrace.size() - 1 && duration > 500)) {
                                        // there has been 1s worth of data
                                        // or 500ms of data and this is the last data point
                                        TargetCircle avgCircle = {{0, 0}, 0};
                                        for (size_t i = 0; i < points.size(); i++) {
                                            avgCircle = avgCircle + TargetCircle{points[i].point, points[i].radius};
                                        }
                                        avgCircle = avgCircle / points.size();

                                        double variance = 0;
                                        for (size_t i = 0; i < points.size(); i++) {
                                            variance += sqrt(D2P(avgCircle.center, points[i].point));
                                        }
                                        variance /= points.size();

                                        if (bestVariance == -1 || variance < bestVariance) {
                                            bestVariance = variance;
                                            bestAvgCircle = avgCircle;
                                        }

                                        points.clear();
                                    } else {
                                        points.push_back(currTP);
                                    }
                                }
                            }

                            if (bestVariance > 0) {
                                calibrationFinished(true, bestAvgCircle.center.x, bestAvgCircle.center.y, bestAvgCircle.radius);
                            } else {
                                calibrationFinished(false, -1, -1, -1);
                            }

                            return;
                        } else {
                            currShotTrace.addTracePoint({ center, millisSinceShotStart, circle.radius });
                        }
                    }
                }

                if (!shotStarted) {
                    // eitherways reset triggered value
                    // if shot has not been started
                    lTriggerTime = 0;
                }

                fprintf(logFile, "\n");
                frameid++;
            }

            video.release();
            stopRecording = true;
            if (sensor != NULL) {
                sensor->stop();
            }

            calibrationFinished(false, -1, -1, -1);
        }
};
