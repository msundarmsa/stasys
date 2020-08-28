#pragma once

#include <opencv2/opencv.hpp>
#include "SystemClock.h"
#include "RecordThread.h"
#include "Utils.h"

class CalibrationThread : public RecordThread {
	private:
		cv::VideoCapture video;
		std::vector<TraceCircle> currentTrace;
        std::function<void(bool, double, double, double)> calibrationFinished;
		cv::SimpleBlobDetector::Params params;
		cv::Ptr<cv::SimpleBlobDetector> detector;
		bool stopRecording = false;
		FILE *logFile;
	public:
        CalibrationThread(cv::VideoCapture video, std::function<void(bool, double, double, double)> calibrationFinished, FILE* logFile) {
			this->video = video;
            this->calibrationFinished = calibrationFinished;
			this->logFile = logFile;

			params.maxThreshold = 100;

			params.filterByArea = true;
			params.minArea = 450;

			params.filterByCircularity = true;
			params.minCircularity = 0.85;

			params.filterByInertia = true;
			params.minInertiaRatio = 0.85;

			detector = cv::SimpleBlobDetector::create(params);
		}

		TargetCircle findCircle(cv::Mat frame)
		{
			TargetCircle resultCircle;
			resultCircle.center = Vector2D { -1.0, -1.0 };
			resultCircle.radius = (double)-1.0;

			cv::Mat grayFrame;
			std::vector<cv::KeyPoint> keypoints;

			cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
			//Imgproc.GaussianBlur(grayFrame, grayFrame, new Size(9, 9), 2, 2 );
			detector->detect(grayFrame, keypoints);

			if (keypoints.size() == 1) {
				resultCircle.center.x = keypoints[0].pt.x;
				resultCircle.center.y = keypoints[0].pt.y;
				resultCircle.radius = keypoints[0].size / 2;
			}

			return resultCircle;
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
			recordThread = new std::thread(&CalibrationThread::run, this);
		}

		bool isRunning()
		{
			return !stopRecording;
		}

	private:
		void findAvgCircle(TargetCircle* avgCircle) {
			TraceCircle currStartTP = currentTrace.at(0);
			Vector2D averages[] = {
				currStartTP.circle.center,
				{ 0, 0 },
				{ 0, 0 }
			};
			int avgCount[] = { 1, 0, 0 };
			int currAvgIndex = 0;
			double avgRadius = currStartTP.circle.radius;

			for (int i = 1; i < currentTrace.size(); i++) {
				TraceCircle currTP = currentTrace.at(i);
				double time_interval = SystemClock::getElapsedSeconds(currTP.time, currStartTP.time);
				if (time_interval > 1) {
					currAvgIndex++;
					if (currAvgIndex > 2) {
						currAvgIndex = 2;
					}
				}

				avgRadius += currTP.circle.radius;
				averages[currAvgIndex] = averages[currAvgIndex] + currTP.circle.center;
				avgCount[currAvgIndex]++;
			}

			if (avgCount[0] > 0 && avgCount[1] > 0 && avgCount[2] > 0) {
				avgRadius = avgRadius / currentTrace.size();

				for (int i = 0; i < 3; i++) {
					averages[i] = { averages[i].x / avgCount[i], averages[i].y / avgCount[i] };
				}

				Vector2D avg = averages[0] + averages[1] + averages[2];
				avg = { avg.x / 3, avg.y / 3 };

				if (D2P(averages[0], avg) < avgRadius &&
						D2P(averages[1], avg) < avgRadius &&
						D2P(averages[2], avg) < avgRadius) {
					avgCircle->center = avg;
					avgCircle->radius = avgRadius;
					return;
				}
			}

			avgCircle->radius = -1;
		}

		void run() {
			cv::Mat frame;

			uint64_t lStartTime;
			uint64_t lCurrTime;

			TargetCircle avgCircle;
			avgCircle.radius = -1;

			int frameId = 0;

			double vecX = 0.0;
			double vecY = 0.0;

			bool success = false;

			// calibration finishes when a valid average of points within 3s is found
			while (!stopRecording && avgCircle.radius == -1) {
				// get frame
				fprintf(logFile, "Frame #%d", frameId);
				video >> frame;

				if (frameId == 0) {
					lStartTime = SystemClock::getCurrentTimeMillis();
				}

				if (frame.empty()) {
					// if camera becomes disconnected, exit
					fprintf(logFile, "\tDropped frame!\n");
					break;
				}

				lCurrTime = SystemClock::getCurrentTimeMillis();

				TargetCircle target = findCircle(frame);
				fprintf(logFile, "\t(%.3f , %.3f) %.3f\n", target.center.x, target.center.y, target.radius);
				if (target.radius != -1) {
					// circle is found
					TraceCircle trace = { target, lCurrTime };

					double totalTraceTime = 0;

					if (currentTrace.size() > 0) {
						totalTraceTime = SystemClock::getElapsedSeconds(lCurrTime, currentTrace.at(0).time);
					}

					currentTrace.push_back(trace);
					if ((totalTraceTime >= 3) && (totalTraceTime <= 3.05)) {
						// we have 3s worth of data
						// see if this produces a valid average
						findAvgCircle(&avgCircle);
					}
					else if (totalTraceTime > 3.05) {
						// we have more than 3s worth of data
						// remove first element from vector until the vector only
						// contains 3s worth of data
						while (currentTrace.size() > 0)
						{
							double currTotalTraceTime = SystemClock::getElapsedSeconds(lCurrTime, currentTrace.at(0).time);
							if (currTotalTraceTime > 3.05 && currentTrace.size() > 0)
							{
								currentTrace.erase(currentTrace.begin(), currentTrace.begin() + 1);
							}
							else
							{
								break;
							}
						}

						// if after removing, we now have 3s worth of data
						// see if this produces a valid average
						double currTotalTraceTime = SystemClock::getElapsedSeconds(lCurrTime, currentTrace.at(0).time);
						if ((currTotalTraceTime >= 3) && (currTotalTraceTime <= 3.05))
						{
							findAvgCircle(&avgCircle);
						}
					}

					if (avgCircle.radius != -1.0) {
						// a valid average was produced
                        vecX = avgCircle.center.x;
                        vecY = avgCircle.center.y;

						// release video, and break
						video.release();
						stopRecording = true;
						success = true;
						break;
					}
				}

                frameId++;
			}

			// callback result
            this->calibrationFinished(success, vecX, vecY, avgCircle.radius);
		}
};
