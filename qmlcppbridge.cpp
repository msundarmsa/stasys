#include "qmlcppbridge.h"
#include "CalibrationThread.h"
#include <opencv2/opencv.hpp>

using namespace std::placeholders;

QMLCppBridge::QMLCppBridge(QObject *parent) : QObject(parent)
{

}

void QMLCppBridge::calibrationClicked()
{
    qDebug() << "Calibration Clicked";
    auto calibrationFinishedPtr = std::bind(&QMLCppBridge::calibrationFinished, this, _1, _2, _3, _4, _5, _6);
    cv::VideoCapture cap("/Users/msundarmsa/stasys/video1_cut.mp4");
    CalibrationThread *thread = new CalibrationThread(cap, calibrationFinishedPtr, stdout);
    thread->start();
}

void QMLCppBridge::shootClicked()
{
    qDebug() << "Shoot Clicked";
}

void QMLCppBridge::calibrationFinished(bool success, double x, double y, double radius, int frameWidth, int frameHeight) {
    emit calibrationCompleted(success);
}
