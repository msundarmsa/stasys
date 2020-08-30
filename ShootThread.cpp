#include "ShootThread.h"
#include "SoundPressureSensor.cpp"
#include "ShotTrace.h"

ShootThread::ShootThread(cv::VideoCapture video, std::string mic, float TRIGGER_DB, double RATIO1, Vector2D adjustmentVec, Vector2D fineAdjustment, ShootController page, FILE* logFile) {
	this->video = video;
	this->page = page;
	this->logFile = logFile;
    this->TRIGGER_DB = TRIGGER_DB;
    this->RATIO1 = RATIO1;
    this->adjustmentVec = adjustmentVec;
    this->fineAdjustment = fineAdjustment;

    params.minThreshold = 100;
    params.maxThreshold = 200;

	params.filterByArea = true;
	params.minArea = 450;

	params.filterByCircularity = true;
	params.minCircularity = 0.85;

	params.filterByInertia = true;
	params.minInertiaRatio = 0.85;

	detector = cv::SimpleBlobDetector::create(params);
        
    sensor = new SoundPressureSensor(this, TRIGGER_DB);
    if (!sensor->setDevice(mic))
    {
        fprintf(logFile, "No Audio Input Device!\n");
        sensor = NULL;
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
    fprintf(logFile, "\tAudio Trigger Received.");
	audio_triggered = true;
}

void ShootThread::start() {
    recordThread = new std::thread(&ShootThread::run, this);
}

void ShootThread::run() {
	cv::Mat frame;
		
    Vector2D preTrace[2] = {
        {-1, -1},
        {-1, -1}
    };

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

    page.removePreviousCalibCircle();

    if (sensor != NULL) {
        sensor->start();
    }

    int testTriggers[10] = {
        1501,
        4474,
        7512,
        10564,
        12949,
        15016,
        17275,
        20238,
        22831,
        25518
    };

    int testTriggerIndex = 0;

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
        
        if (sensor == NULL && testTriggerIndex < 10 && frameid == testTriggers[testTriggerIndex]) {
            audio_triggered = true;
            testTriggerIndex++;
        }

        lFrameTime = SystemClock::getCurrentTimeMillis();
        double timeSinceShotStart = SystemClock::getElapsedSeconds(lFrameTime, lShotStartTime);
		//calculateFPS(currTime);

        if (shotStarted) {
            // shot has started i.e. the aim has went past the top edge and came back down
            double timeSinceCircleDetected = SystemClock::getElapsedSeconds(lFrameTime, lLastCircleDetectedFrameTime);
            if (timeSinceCircleDetected > 2.0) {
                // reset if shot has started but aim is not within the target/cannot be found
                // for 2s
                shotStarted = false;
                currShotTrace.reset();
                preTrace[0] = { -1, -1 };
                preTrace[1] = { -1, -1 };
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
                else if (currShotTrace.afterShot() && relativeFrameTime >=
                    currShotTrace.getShotPoint().time + 2.0)
                {
                    
                    // 2s after trigger is pulled, shot is finished. create new object for this shot
                    // and draw the x-t and y-t graph
                    Shot* shot = new Shot(++sn, currShotTrace);
                    page.updateView(shot);

                    // reset shot
                    shotStarted = false;
                    preTrace[0] = { -1, -1 };
                    preTrace[1] = { -1, -1 };
                    currShotTrace.reset();
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
			double xShift = (round(circle.center.y) - roi.height / 2) * RATIO1 + fineAdjustment.x;
			double yShift = (-round(circle.center.x) + roi.width / 2) * RATIO1 + fineAdjustment.y;
            Vector2D center = { xShift, yShift };
            fprintf(logFile, "\t(%.3f , %.3f)", center.x, center.y);

            if (center.x >= -roi.width / 2 && center.x <= roi.width / 2 &&
                center.y >= -roi.width / 2 && center.y <= roi.width / 2) {
                // aim is found and within the target
                lLastCircleDetectedFrameTime = lFrameTime;
            }

            if (!shotStarted) {
                if (preTrace[1].x == -1) {
                    preTrace[1] = center;
                }
                else {
                    preTrace[0] = preTrace[1];
                    preTrace[1] = center;

                    shotStarted = (preTrace[0].y > TARGET_SIZE / 2 && preTrace[1].y < TARGET_SIZE / 2);
                    // shot is started if the aim went past the edge (preTrace[0].y < 0)
                    // and came back down after that (preTrace[1].y > 0)
                    if (shotStarted) {
                        // new shot started

                        // reset traces
                        currShotTrace.reset();
                        page.clearTrace(true);

                        lShotStartTime = lFrameTime;
                        page.addToBeforeShotTrace(center);
                        currShotTrace.addTracePoint({ center, 0 });
                    }
                }
            }
            else {
                if (!currShotTrace.afterShot()) {
                    if (audio_triggered) {
                        // trigger has just been pulled
                        audio_triggered = false;
                        fprintf(logFile, "\tTRIGGER RECEIVED");

                        currShotTrace.setShotPoint({ center, timeSinceShotStart });

                        page.addToBeforeShotTrace(center);
                        page.drawShotCircle(center);
                        page.addToAfterShotTrace(center);
                    }
                    else {
                        // trigger has not been pulled
                        currShotTrace.addTracePoint({ center, timeSinceShotStart });
                        page.addToBeforeShotTrace(center);
                    }
                }
                else {
                    // trigger has been pulled
                    currShotTrace.addTracePoint({ center, timeSinceShotStart });
                    page.addToAfterShotTrace(center);
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
