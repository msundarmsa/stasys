#include "RecordThread.h"
#include "SoundPressureSensor.cpp"

using namespace std;
using namespace cv;

RecordThread::RecordThread(float TRIGGER_DB, string mic)
{
    SimpleBlobDetector::Params params;
    params.minThreshold = 30;
    params.maxThreshold = 80;

    params.filterByArea = true;
    params.minArea = 450;

    params.filterByCircularity = true;
    params.minCircularity = 0.85;

    params.filterByInertia = true;
    params.minInertiaRatio = 0.85;

    detector = SimpleBlobDetector::create(params);

    if (TRIGGER_DB > 0) {
        sensor = new SoundPressureSensor(this, TRIGGER_DB);
        if (!sensor->setDevice(mic))
        {
            sensor = NULL;
        }
    }
}

TargetCircle RecordThread::findCircle(Mat frame)
{
    TargetCircle resultCircle = {{-1, -1}, -1};

    Mat grayFrame;
    vector<KeyPoint> keypoints;

    cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
    //cv::GaussianBlur(grayFrame, grayFrame, new Size(9, 9), 2, 2 );
    detector->detect(grayFrame, keypoints);

    if (keypoints.size() == 1) {
        resultCircle.center.x = keypoints[0].pt.x;
        resultCircle.center.y = keypoints[0].pt.y;
        resultCircle.radius = keypoints[0].size / 2;
    }

    return resultCircle;
}

void RecordThread::start()
{
    recordThread = new thread(&RecordThread::run, this);
}

void RecordThread::stop()
{
}

bool RecordThread::isRunning()
{
    return recordThread->joinable();
}

void RecordThread::run()
{
}

void RecordThread::join()
{
	recordThread->join();
}

void RecordThread::audio_trigger(uint64_t trigger_time) {
    this->lTriggerTime = trigger_time;
}
