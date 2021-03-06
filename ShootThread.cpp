#include "ShootThread.h"
#include "SoundPressureSensor.cpp"
#include "ShotTrace.h"
#include "spline.h"

using namespace std;
using namespace cv;

ShootThread::ShootThread(int startSn, VideoCapture video, int mic_index, bool upDownDetection, float TRIGGER_DB, int minThreshold, int maxThreshold, double RATIO1, Vector2D adjustmentVec, Vector2D fineAdjustment, ShootController page, FILE* logFile) : RecordThread(TRIGGER_DB, mic_index, minThreshold, maxThreshold) {
    this->sn = startSn;
    this->video = video;
	this->page = page;
	this->logFile = logFile;
    this->RATIO1 = RATIO1;
    this->adjustmentVec = adjustmentVec;
    this->fineAdjustment = fineAdjustment;
    this->upDownDetection = upDownDetection;
}

void ShootThread::start() {
    recordThread = new thread(&ShootThread::run, this);
}

void ShootThread::run() {
    Mat frame;

    Vector2D preTrace[2];
    int preTraceIndex = 0;

    ShotTrace currShotTrace;

	uint64_t lStartTime = 0;
	uint64_t lFrameTime = 0;
    uint64_t lShotStartTime = 0;
	uint64_t lLastCircleDetectedFrameTime = 0;

    int frameid = 0;

    bool shotStarted = false;

    #ifdef QT_QML_DEBUG
        fprintf(logFile, "RATIO1 (mm / px) %.3f\n", RATIO1);
        fprintf(logFile, "Adjustment Vec (px, px): (%.3f, %.3f)\n", adjustmentVec.x, adjustmentVec.y);
        fprintf(logFile, "Fine Adjustment Vec (mm, mm): (%.3f, %.3f)\n", fineAdjustment.x, fineAdjustment.y);
        fprintf(logFile, "Up/Down Detection: (%s)\n", upDownDetection ? "true" : "false");
    #else
        fprintf(logFile, "params,%.3f,%.3f,%.3f,%.3f,%.3f,%d\n", RATIO1, adjustmentVec.x, adjustmentVec.y, fineAdjustment.x, fineAdjustment.y, upDownDetection);
    #endif

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

    while (!stopRecording) {
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
                page.clearTrace(false);
            }
            else {
                if (millisSinceShotStart > 60000 && !currShotTrace.isShotPointSet())
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
                }
            }
        }

        // clip 1.75x size of card around aim center
        Rect roi;
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
        #ifdef QT_QML_DEBUG
            fprintf(logFile, "\t[%s]", (shotStarted ? "true" : "false"));
        #else
            fprintf(logFile, ",%d", shotStarted);
        #endif
        if (circle.radius != -1) {
            // aim i.e. black circle was found
			// flip & rotate the x, y to fit camera
            double xShift = (circle.center.y - roi.height / 2) * RATIO1 + fineAdjustment.x;
            double yShift = (-circle.center.x + roi.width / 2) * RATIO1 + fineAdjustment.y;
            Vector2D center = { xShift, yShift };
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
                            currShotTrace.addTracePoint({ center, millisSinceShotStart, circle.radius });
                            page.addToBeforeShotTrace(center);
                            currShotTrace.setTriggerPulled();
                        } else {
                            // trigger was before frame was taken
                            // add current position to after trace
                            currShotTrace.setTriggerPulled();
                            currShotTrace.addTracePoint({ center, millisSinceShotStart, circle.radius });
                        }

                        #ifdef QT_QML_DEBUG
                            fprintf(logFile, "\tTRIGGER RECEIVED AT %llu", lTriggerTime);
                        #else
                            fprintf(logFile, ",%llu", lTriggerTime);
                        #endif
                    } else {
                        currShotTrace.addTracePoint({ center, millisSinceShotStart, circle.radius });
                        page.addToBeforeShotTrace(center);
                    }
                } else {
                    if (currShotTrace.getAfterShotTrace().size() < 2) {
                        currShotTrace.addTracePoint({ center, millisSinceShotStart, circle.radius });
                    } else if (currShotTrace.getAfterShotTrace().size() == 2) {
                        currShotTrace.addTracePoint({ center, millisSinceShotStart, circle.radius });

                        // use spline to calculate actual shot point
                        vector<double> X, Y, T;
                        vector<TracePoint> beforeShotTrace = currShotTrace.getBeforeShotTrace();
                        vector<TracePoint> afterShotTrace = currShotTrace.getAfterShotTrace();
                        for (size_t i = beforeShotTrace.size() - 3; i < beforeShotTrace.size(); i++) {
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

                        #ifdef QT_QML_DEBUG
                            fprintf(logFile, "\t{%.3f , %.3f}", shotPoint.x, shotPoint.y);
                        #else
                            fprintf(logFile, ",%.3f,%.3f", shotPoint.x, shotPoint.y);
                        #endif
                        currShotTrace.setShotPoint({ shotPoint, trigger_time, circle.radius });
                        page.addToBeforeShotTrace(shotPoint);
                        page.drawShotCircle(shotPoint);
                        page.addToAfterShotTrace(shotPoint);

                        lTriggerTime = 0;
                    } else {
                        currShotTrace.addTracePoint({ center, millisSinceShotStart, circle.radius });
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
    }
}

bool ShootThread::isRunning()
{
    return !stopRecording;
}
