#include "qmlcppbridge.h"
#include <opencv2/opencv.hpp>
#include <SFML/Audio.hpp>
#include <QCameraInfo>

using namespace std::placeholders;

QMLCppBridge::QMLCppBridge(QObject *parent) : QObject(parent)
{
    std::vector<std::string> availableDevices = sf::SoundRecorder::getAvailableDevices();
    currentMic = availableDevices[0];
}

void QMLCppBridge::settingsOpened()
{
    std::vector<std::string> availableDevices = sf::SoundRecorder::getAvailableDevices();
    QStringList micOptions = {QString::fromStdString(currentMic)};
    for (int i = 0; i < availableDevices.size(); i++) {
        if (availableDevices[i] != currentMic) {
            micOptions.append(QString::fromStdString(availableDevices[i]));
        }
    }

    const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    QStringList cameraOptions = {cameras[CAMERA_INDEX].description()};
    for (int i = 0; i < cameras.size(); i++){
        if (i != CAMERA_INDEX) {
            cameraOptions.append(cameras[i].description());
        }
    }

    auto updateSamplesPtr = std::bind(&QMLCppBridge::updateSamples, this, _1);
    micThread = new MicThread(updateSamplesPtr);
    micThread->setDevice(availableDevices[0]);
    micThread->start();
    emit uiSettingsOpened(micOptions, QString::fromStdString(currentMic), TRIGGER_DB, cameraOptions, CAMERA_INDEX);
}

void QMLCppBridge::settingsClosed()
{
    micThread->stop();
    delete micThread;
    micThread = NULL;
}

void QMLCppBridge::updateSamples(float dB)
{
    emit uiUpdateSamples(dB);
}

void QMLCppBridge::micChanged(QString newMic)
{
    currentMic = newMic.toStdString();
    micThread->stop();
    micThread->setDevice(currentMic);
    micThread->start();
}

void QMLCppBridge::micThresholdChanged(float newThreshold)
{
    TRIGGER_DB = newThreshold;
}

void QMLCppBridge::cameraChanged(int camera)
{
    CAMERA_INDEX = camera;
}

void QMLCppBridge::stopRecording()
{
    if (calibThread != NULL) {
        calibThread->stop();
        emit uiCalibrationEnded(false);
    }

    if (shootThread != NULL) {
        shootThread->stop();
        emit uiShootingEnded();
    }
}

void QMLCppBridge::adjustCalibration(double deltaX, double deltaY)
{
    fineAdjustment.x += deltaX;
    fineAdjustment.y += deltaY;
}

void QMLCppBridge::calibrationClicked()
{
    if (calibThread == NULL && shootThread == NULL) {
        //cv::VideoCapture cap("/Users/msundarmsa/stasys/5x calibration.mp4");
        cv::VideoCapture cap(CAMERA_INDEX);

        auto calibrationFinishedPtr = std::bind(&QMLCppBridge::calibrationFinished, this, _1, _2, _3, _4);
        calibThread = new CalibrationThread(cap, calibrationFinishedPtr, stdout);
        calibThread->start();
        emit uiCalibrationStarted();
    } else if (calibThread != NULL) {
        calibThread->stop();
        calibThread = NULL;
    } else {
        emit uiShowToast("Please stop shooting by pressing the 'SHOOTING' button before trying to calibrate");
    }
}

void QMLCppBridge::shootClicked()
{
    if (calibThread == NULL && shootThread == NULL) {
        //cv::VideoCapture cap("/Users/msundarmsa/stasys/10x shots.mp4");
        cv::VideoCapture cap(CAMERA_INDEX);

        auto removePreviousCalibCirclePtr = std::bind(&QMLCppBridge::removePreviousCalibCircle, this);
        auto clearTracePtr = std::bind(&QMLCppBridge::clearTrace, this, _1);
        auto updateViewPtr = std::bind(&QMLCppBridge::updateView, this, _1);
        auto addToBeforeShotTracePtr = std::bind(&QMLCppBridge::addToBeforeShotTrace, this, _1);
        auto drawShotCirclePtr = std::bind(&QMLCppBridge::drawShotCircle, this, _1);
        auto addToAfterShotTracePtr = std::bind(&QMLCppBridge::addToAfterShotTrace, this, _1);
        ShootController controller = { removePreviousCalibCirclePtr, clearTracePtr, updateViewPtr, addToBeforeShotTracePtr, drawShotCirclePtr, addToAfterShotTracePtr };

        shootThread = new ShootThread(cap, currentMic, TRIGGER_DB, RATIO1, adjustmentVec, fineAdjustment, controller, stdout);
        shootThread->start();
        emit uiShootingStarted();
    } else if (shootThread != NULL) {
        shootThread->stop();
        shootThread = NULL;
        emit uiShootingEnded();
    } else {
        emit uiShowToast("Please stop calibrating by pressing the 'CALIBRATING' button before trying to shoot");
    }
}

void QMLCppBridge::calibrationFinished(bool success, double x, double y, double radius) {
    calibThread = NULL;
    adjustmentVec = {x, y};
    RATIO1 = PISTOL_CIRCLE_SIZE / radius;
    emit uiCalibrationEnded(success);
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
