#include "ShootThread.h"
#include "SoundPressureSensor.cpp"
#include "ShotTrace.h"
#include "spline.h"

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

void ShootThread::audio_trigger(uint64_t trigger_time) {
    this->lTriggerTime = trigger_time;
}

void ShootThread::start() {
    recordThread = new std::thread(&ShootThread::run, this);
}

void ShootThread::run() {
	cv::Mat frame;

    Vector2D preTrace[2];
    int preTraceIndex = 0;

    ShotTrace currShotTrace;

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
		video >> frame;
        lFrameTime = SystemClock::getCurrentTimeMillis();

        if (frame.empty()) {
            // if camera becomes disconnected, exit
            fprintf(logFile, "\tDropped frame!\n");
            break;
        }

        fprintf(logFile, "%" PRIu64 "", lFrameTime);

		if (frameid == 0) {
            lStartTime = lFrameTime;
        }

        #ifdef QT_QML_DEBUG
            if (sensor == NULL && testTriggerIndex < 30 && frameid == testTriggers[testTriggerIndex]) {
                lTriggerTime = lFrameTime;
                testTriggerIndex++;
            }
        #endif

        double millisSinceShotStart = SystemClock::getElapsedMillis(lFrameTime, lShotStartTime);

        if (shotStarted) {
            // shot has started i.e. the aim has went past the top edge and came back down
            double millisSinceCircleDetected = SystemClock::getElapsedMillis(lFrameTime, lLastCircleDetectedFrameTime);
            if (millisSinceCircleDetected > 2000) {
                // reset shot if shot has started but aim is not within the target/cannot be found
                // for 2s
                shotStarted = false;
                currShotTrace.reset();
                preTraceIndex = 0;
                page.clearTrace(false);
            }
            else {
                if (millisSinceShotStart > 60000)
                {
                    // reset trace if shot has started but trigger has not been pulled for 60s
                    currShotTrace.reset();
                    page.clearTrace(false);

                    // but update the start time to the current time
                    lShotStartTime = lFrameTime;
                }
                else if (currShotTrace.isShotPointSet() && millisSinceShotStart >=
                    currShotTrace.getShotPoint().time + 1000)
                {
                    // 1s after trigger is pulled, shot is finished. create new object for this shot
                    // and draw the x-t and y-t graph
                    Shot* shot = new Shot(++sn, currShotTrace);
                    page.updateView(shot);

                    // reset shot
                    shotStarted = false;
                    currShotTrace.reset();
                    preTraceIndex = 0;
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
                } else if (upDownDetection) {
                    // move pretrace window down
                    preTrace[0] = preTrace[1];
                    preTraceIndex = 1;
                }
            } else {
                if (!currShotTrace.afterShot()) {
                    if (lTriggerTime != 0) {
                        // trigger has just been pulled
                        if (lTriggerTime > lFrameTime) {
                            // trigger was after frame was taken
                            // add current position to before trace
                            currShotTrace.addTracePoint({ center, millisSinceShotStart });
                            page.addToBeforeShotTrace(center);
                            currShotTrace.setTriggerPulled();
                        } else {
                            // trigger was before frame was taken
                            // add current position to after trace
                            currShotTrace.setTriggerPulled();
                            currShotTrace.addTracePoint({ center, millisSinceShotStart });
                        }
                        fprintf(logFile, "\tTRIGGER RECEIVED AT %llu", lTriggerTime);
                    } else {
                        currShotTrace.addTracePoint({ center, millisSinceShotStart });
                        page.addToBeforeShotTrace(center);
                    }
                } else {
                    if (currShotTrace.getAfterShotTrace().size() < 2) {
                        currShotTrace.addTracePoint({ center, millisSinceShotStart });
                    } else if (currShotTrace.getAfterShotTrace().size() == 2) {
                        currShotTrace.addTracePoint({ center, millisSinceShotStart });

                        // use spline to calculate actual shot point
                        std::vector<double> X, Y, T;
                        std::vector<TracePoint> beforeShotTrace = currShotTrace.getBeforeShotTrace();
                        std::vector<TracePoint> afterShotTrace = currShotTrace.getAfterShotTrace();
                        for (int i = beforeShotTrace.size() - 3; i < beforeShotTrace.size(); i++) {
                            X.push_back(beforeShotTrace[i].point.x);
                            Y.push_back(beforeShotTrace[i].point.y);
                            T.push_back(beforeShotTrace[i].time);
                        }

                        for (int i = 0; i < 3; i++) {
                            X.push_back(afterShotTrace[i].point.x);
                            Y.push_back(afterShotTrace[i].point.y);
                            T.push_back(afterShotTrace[i].time);
                        }

                        tk::spline sx, sy;
                        sx.set_points(T, X);
                        sy.set_points(T, Y);

                        double trigger_time = SystemClock::getElapsedMillis(lTriggerTime, lShotStartTime) + TIME_OFFSET;

                        Vector2D interpPoint = { sx(trigger_time), sy(trigger_time) };
                        Vector2D velocity = { sx.deriv(1, trigger_time), sy.deriv(1, trigger_time) };
                        Vector2D shotPoint = interpPoint + velocity * VELOCITY_FACTOR;

                        fprintf(logFile, "\t{%.3f , %.3f}", shotPoint.x, shotPoint.y);
                        currShotTrace.setShotPoint({ shotPoint, trigger_time });
                        page.addToBeforeShotTrace(shotPoint);
                        page.drawShotCircle(shotPoint);
                        page.addToAfterShotTrace(shotPoint);

                        lTriggerTime = 0;
                    } else {
                        currShotTrace.addTracePoint({ center, millisSinceShotStart });
                        page.addToAfterShotTrace(center);
                    }
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
