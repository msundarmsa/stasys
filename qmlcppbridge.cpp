#include "qmlcppbridge.h"
#include "ShootThread.h"
#include "CalibrationThread.h"
#include <opencv2/opencv.hpp>

using namespace std::placeholders;

QMLCppBridge::QMLCppBridge(QObject *parent) : QObject(parent)
{

}

void QMLCppBridge::calibrationClicked()
{
    auto calibrationFinishedPtr = std::bind(&QMLCppBridge::calibrationFinished, this, _1, _2, _3, _4, _5, _6);
    cv::VideoCapture cap("/Users/msundarmsa/stasys/5x calibration.mp4");
    CalibrationThread *thread = new CalibrationThread(cap, calibrationFinishedPtr, stdout);
    thread->start();
}

void QMLCppBridge::shootClicked()
{
    cv::VideoCapture cap("/Users/msundarmsa/stasys/10x shots.mp4");

    auto removePreviousCalibCirclePtr = std::bind(&QMLCppBridge::removePreviousCalibCircle, this);
    auto clearTracePtr = std::bind(&QMLCppBridge::clearTrace, this, _1);
    auto updateViewPtr = std::bind(&QMLCppBridge::updateView, this, _1);
    auto addToBeforeShotTracePtr = std::bind(&QMLCppBridge::addToBeforeShotTrace, this, _1);
    auto drawShotCirclePtr = std::bind(&QMLCppBridge::drawShotCircle, this, _1);
    auto addToAfterShotTracePtr = std::bind(&QMLCppBridge::addToAfterShotTrace, this, _1);
    ShootController controller = { removePreviousCalibCirclePtr, clearTracePtr, updateViewPtr, addToBeforeShotTracePtr, drawShotCirclePtr, addToAfterShotTracePtr };

    if (radius == 0) {
        radius = 18.469;
        adjustmentVec = {388.951, 324.785};
    }

    ShootThread *thread = new ShootThread(cap, radius, adjustmentVec, controller, stdout);
    thread->start();
}

void QMLCppBridge::calibrationFinished(bool success, double x, double y, double radius, int frameWidth, int frameHeight) {
    adjustmentVec = {x, y};
    this->radius = radius;
    emit calibrationCompleted(success);
}

void QMLCppBridge::removePreviousCalibCircle() {
    emit uiRemovePreviousCalibCircle();
}

void QMLCppBridge::clearTrace(bool resetGroupIfNecessary) {
    emit uiClearTrace(resetGroupIfNecessary);
}

void QMLCppBridge::updateView(Shot* shot) {
    Vector2D center = shot->getShotPoint().point;
    double score = shot->getScore();
    double stab = shot->getStab();
    double desc = shot->getDesc();
    double aim = shot->getAim();

    ShotTrace shotTrace = shot->getShotTrace();
    std::vector<TracePoint> beforeTrace = shotTrace.getBeforeShotTrace();
    TracePoint shotPoint = shotTrace.getShotPoint();
    std::vector<TracePoint> afterTrace = shotTrace.getAfterShotTrace();

    int frames = 0.5 * FPS;
    QList<double> xtList = {};
    QList<double> ytList = {};
    QList<double> tList = {};
    for (int i = frames; i >= 1; i--) {
        int pos = beforeTrace.size() - i;
        xtList.append(beforeTrace[pos].point.x);
        ytList.append(beforeTrace[pos].point.y);
        tList.append(-(double)i / (double)FPS);
    }

    xtList.append(shotPoint.point.x);
    ytList.append(shotPoint.point.y);
    tList.append(0);

    for (int i = 0; i < frames; i++) {
        xtList.append(afterTrace[i].point.x);
        ytList.append(afterTrace[i].point.y);
        tList.append((double)(i + 1) / (double)FPS);
    }

    QVariant xt = QVariant::fromValue(xtList);
    QVariant yt = QVariant::fromValue(ytList);
    QVariant ts = QVariant::fromValue(tList);

    emit uiUpdateView(center.x, center.y, score, stab, desc, aim, xt, yt, ts);
}

void QMLCppBridge::addToBeforeShotTrace(Vector2D center) {
    emit uiAddToBeforeShotTrace(center.x, center.y);
}

void QMLCppBridge::drawShotCircle(Vector2D center) {
    emit uiDrawShotCircle(center.x, center.y);
}

void QMLCppBridge::addToAfterShotTrace(Vector2D center) {
    emit uiAddToAfterShotTrace(center.x, center.y);
}
