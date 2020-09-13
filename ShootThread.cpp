#include "ShootThread.h"
#include "SoundPressureSensor.cpp"
#include "ShotTrace.h"

ShootThread::ShootThread(int startSn, cv::VideoCapture video, std::string mic, bool upDownDetection, float TRIGGER_DB, double RATIO1, Vector2D adjustmentVec, Vector2D fineAdjustment, ShootController page, FILE* logFile) {
    this->sn = startSn;
    this->video = video;
	this->page = page;
	this->logFile = logFile;
    this->TRIGGER_DB = TRIGGER_DB;
    this->RATIO1 = RATIO1;
    this->adjustmentVec = adjustmentVec;
    this->fineAdjustment = fineAdjustment;
    this->upDownDetection = upDownDetection;

    params.minThreshold = 100;
    params.maxThreshold = 200;

	params.filterByArea = true;
	params.minArea = 450;

	params.filterByCircularity = true;
	params.minCircularity = 0.85;

	params.filterByInertia = true;
	params.minInertiaRatio = 0.85;

	detector = cv::SimpleBlobDetector::create(params);
        
    if (TRIGGER_DB > 0) {
        sensor = new SoundPressureSensor(this, TRIGGER_DB);
        if (!sensor->setDevice(mic))
        {
            fprintf(logFile, "No Audio Input Device!\n");
            sensor = NULL;
        }
    }
}

TargetCircle ShootThread::findCircle(cv::Mat frame)
{
	TargetCircle resultCircle;
	resultCircle.center = Vector2D{ -1.0, -1.0 };
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

void ShootThread::audio_trigger() {
	audio_triggered = true;
}

void ShootThread::start() {
    recordThread = new std::thread(&ShootThread::run, this);
}

void ShootThread::run() {
	cv::Mat frame;

    Vector2D preTrace[2];
    int preTraceIndex = 0;

    ShotTrace currShotTrace;
    TracePoint movingAvg[3];
    int movingAvgIndex = 0;

	uint64_t lStartTime = 0;
	uint64_t lFrameTime = 0;
    uint64_t lShotStartTime = 0;
	uint64_t lLastCircleDetectedFrameTime = 0;

    int frameid = 0;

    bool shotStarted = false;
		
    fprintf(logFile, "RATIO1 (mm / px) %.3f\n", RATIO1);
    fprintf(logFile, "Adjustment Vec (px, px): (%.3f, %.3f)\n", adjustmentVec.x, adjustmentVec.y);
    fprintf(logFile, "Fine Adjustment Vec (mm, mm): (%.3f, %.3f)\n", fineAdjustment.x, fineAdjustment.y);
    fprintf(logFile, "Up/Down Detection: (%s)\n", upDownDetection ? "true" : "false");
    fprintf(logFile, "TRIGGER: (%.3f)\n", TRIGGER_DB);

    page.removePreviousCalibCircle();

    if (sensor != NULL) {
        sensor->start();
    }

    #ifdef QT_QML_DEBUG
        int testTriggers[30] = {
            312,
            1754,
            3295,
            5116,
            6506,
            7589,
            9914,
            11104,
            12424,
            14422,
            15713,
            17499,
            21796,
            23156,
            24537,
            26312,
            27402,
            28559,
            29944,
            31059,
            32233,
            33654,
            34582,
            35775,
            37593,
            38745,
            40029,
            41795,
            43269,
            44616
        };

        int testTriggerIndex = 0;
    #endif

	while (!stopRecording && (frameid == 0 || !frame.empty())) {
		fprintf(logFile, "Frame #%d", frameid);
		video >> frame;

        if (frame.empty()) {
            // if camera becomes disconnected, exit
            fprintf(logFile, "\tDropped frame!\n");
            break;
        }

		if (frameid == 0) {
            lStartTime = SystemClock::getCurrentTimeMillis();
        }

        #ifdef QT_QML_DEBUG
            if (sensor == NULL && testTriggerIndex < 30 && frameid == testTriggers[testTriggerIndex]) {
                audio_triggered = true;
                testTriggerIndex++;
            }
        #endif

        lFrameTime = SystemClock::getCurrentTimeMillis();
        double timeSinceShotStart = SystemClock::getElapsedSeconds(lFrameTime, lShotStartTime);

        if (shotStarted) {
            // shot has started i.e. the aim has went past the top edge and came back down
            double timeSinceCircleDetected = SystemClock::getElapsedSeconds(lFrameTime, lLastCircleDetectedFrameTime);
            if (timeSinceCircleDetected > 2.0) {
                // reset shot if shot has started but aim is not within the target/cannot be found
                // for 2s
                shotStarted = false;
                currShotTrace.reset();
                preTraceIndex = 0;
                movingAvgIndex = 0;
                page.clearTrace(false);
            }
            else {
                double relativeFrameTime = timeSinceShotStart;

                if (relativeFrameTime > 60)
                {
                    // reset trace if shot has started but trigger has not been pulled for 60s
                    currShotTrace.reset();
                    page.clearTrace(false);

                    // but update the start time to the current time
                    lShotStartTime = lFrameTime;
                }
                else if (currShotTrace.isShotPointSet() && relativeFrameTime >=
                    currShotTrace.getShotPoint().time + 1.0)
                {
                    // 1s after trigger is pulled, shot is finished. create new object for this shot
                    // and draw the x-t and y-t graph
                    Shot* shot = new Shot(++sn, currShotTrace);
                    page.updateView(shot);

                    // reset shot
                    shotStarted = false;
                    currShotTrace.reset();
                    preTraceIndex = 0;
                    movingAvgIndex = 0;
                }
            }
        }

        // clip 1.75x size of card around aim center
        cv::Rect roi;
        roi.width = 1.75 * TARGET_SIZE / RATIO1;
        roi.height = roi.width;

        roi.x = adjustmentVec.x - roi.width/2;
        roi.y = adjustmentVec.y - roi.height/2;

        // clip x, y, width, height to be within the bounds of the frame
        roi.x = roi.x < 0 ? 0 : roi.x;
        roi.y = roi.y < 0 ? 0 : roi.y;
        roi.width = roi.x + roi.width > frame.cols ? frame.cols - roi.x : roi.width;
        roi.height = roi.y + roi.height > frame.rows ? frame.rows - roi.y : roi.height;

		frame = frame(roi);

        TargetCircle circle = findCircle(frame);
        fprintf(logFile, "\t[%s]", (shotStarted ? "true" : "false"));
        if (circle.radius != -1) {
            // aim i.e. black circle was found
			// flip & rotate the x, y to fit camera
            double xShift = (circle.center.y - roi.height / 2) * RATIO1 + fineAdjustment.x;
            double yShift = (-circle.center.x + roi.width / 2) * RATIO1 + fineAdjustment.y;
            Vector2D center = { xShift, yShift };
            fprintf(logFile, "\t(%.3f , %.3f)", center.x, center.y);

            if (center.x >= -TARGET_SIZE / 2 && center.x <= TARGET_SIZE / 2 &&
                center.y >= -TARGET_SIZE / 2 && center.y <= TARGET_SIZE / 2) {
                // aim is found and within the target
                lLastCircleDetectedFrameTime = lFrameTime;
            }

            if (!shotStarted) {
                if (upDownDetection) {
                    // if up/down detection is enabled, detect aim going up and down
                    preTrace[preTraceIndex] = center;
                    preTraceIndex++;

                    if (preTraceIndex == 2) {
                        // shot is started if the aim went past the edge (preTrace[0].y > TARGET_SIZE / 2)
                        // and came back down after that (preTrace[1].y < TARGET_SIZE / 2)
                        shotStarted = (preTrace[0].y > TARGET_SIZE / 2 && preTrace[1].y < TARGET_SIZE / 2);
                    }
                } else {
                    // else shot is started from the frame the circle is detected
                    shotStarted = true;
                }

                if (shotStarted) {
                    // new shot started

                    // reset traces
                    currShotTrace.reset();
                    page.clearTrace(true);

                    lShotStartTime = lFrameTime;
                    preTraceIndex = 0;
                    movingAvg[0] = {center, 0};
                    movingAvgIndex = 1;
                } else if (upDownDetection) {
                    // move pretrace window down
                    preTrace[0] = preTrace[1];
                    preTraceIndex = 1;
                }
            } else {
                movingAvg[movingAvgIndex] = {center, timeSinceShotStart};
                movingAvgIndex++;

                if (movingAvgIndex == 3) {
                    Vector2D avgCenter = (movingAvg[0].point + movingAvg[1].point + movingAvg[2].point) / 3;
                    fprintf(logFile, "\t[%.3f, %.3f]", avgCenter.x, avgCenter.y);

                    if (!currShotTrace.afterShot()) {
                        currShotTrace.addTracePoint({ avgCenter, movingAvg[1].time });
                        page.addToBeforeShotTrace(avgCenter);

                        if (audio_triggered) {
                            // trigger has just been pulled
                            audio_triggered = false;
                            fprintf(logFile, "\tTRIGGER RECEIVED");
                            currShotTrace.setTriggerPulled();
                        }
                    } else {
                        if (currShotTrace.getAfterShotTrace().size() == 0) {
                            if (!currShotTrace.isShotPointSet()) {
                                // trigger has been pulled 1 frame ago
                                // shot point has not been set
                                // set average center as shot point first
                                currShotTrace.setShotPoint({ avgCenter, movingAvg[1].time });
                            } else {
                                // trigger was just pulled 2 frames ago
                                // first frame after shot has been smoothed (current avgCenter)
                                // calculate velocity and scale by the velocity factor
                                Vector2D velocity = (avgCenter - currShotTrace.getBeforeShotTrace()[currShotTrace.getBeforeShotTrace().size() - 1].point) / 2;
                                Vector2D shotPoint = velocity + currShotTrace.getShotPoint().point;
                                page.addToBeforeShotTrace(shotPoint);
                                page.drawShotCircle(shotPoint);
                                page.addToAfterShotTrace(shotPoint);

                                fprintf(logFile, "\t{%.3f , %.3f}", shotPoint.x, shotPoint.y);

                                // set correct shotpoint on trace
                                currShotTrace.setShotPoint({ shotPoint, currShotTrace.getShotPoint().time });
                                currShotTrace.addTracePoint({ avgCenter, movingAvg[1].time });
                                page.addToAfterShotTrace(avgCenter);
                            }
                        } else {
                            currShotTrace.addTracePoint({ avgCenter, movingAvg[1].time });
                            page.addToAfterShotTrace(avgCenter);
                        }
                    }

                    movingAvg[0] = movingAvg[1];
                    movingAvg[1] = movingAvg[2];
                    movingAvgIndex = 2;
                }
            }
        }

        if (!shotStarted) {
            // eitherways reset triggered value
            // if shot has not been started
            audio_triggered = false;
        }

        fprintf(logFile, "\n");
        frameid++;
	}
    
    video.release();
    stopRecording = true;
    if (sensor != NULL) {
        sensor->stop();
    }
}

void ShootThread::stop()
{
    if (!stopRecording) {
        stopRecording = true;
        if (recordThread->joinable()) {
            recordThread->join();
        }
    }
}

bool ShootThread::isRunning()
{
    return !stopRecording;
}
