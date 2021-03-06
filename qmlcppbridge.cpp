#include "qmlcppbridge.h"
#include <opencv2/opencv.hpp>
#include <QCameraInfo>
#include <time.h>
#include <QCoreApplication>
#include <QAudioDeviceInfo>
#include <cmath>
#include <algorithm>
#include <portaudio.h>

#define _USE_MATH_DEFINES

using namespace std;
using namespace placeholders;
using namespace cv;

QMLCppBridge::QMLCppBridge(QObject *parent) : QObject(parent)
{
    shots.clear();
    #ifdef QT_QML_DEBUG
        upDownDetection = false;
    #else
        time_t currentTime = chrono::system_clock::to_time_t(chrono::system_clock::now());
        char timestamp[20];
        strftime (timestamp, 20, "%m%d%y_%H%M%S", localtime(&currentTime));
        stringstream filename;
        #ifdef Q_OS_MACX
            QString currentDir = QCoreApplication::applicationDirPath(); // /<something>/stasys-qt.app/Contents/MacOS
            int pos = currentDir.lastIndexOf(QChar('/'));
            currentDir = currentDir.left(pos); // /<something>/stasys-qt.app/Contents
            pos = currentDir.lastIndexOf(QChar('/'));
            currentDir = currentDir.left(pos); // /<something>/stasys-qt.app
            pos = currentDir.lastIndexOf(QChar('/'));
            currentDir = currentDir.left(pos); // /<something>
            filename << currentDir.toStdString() << "/";
        #endif
        filename << "STASYSLog_" << timestamp << ".txt";
        logFile = fopen (filename.str().c_str() , "w");
        if (logFile == NULL) {
            logFile = stdout;
            qDebug() << "Could not open log file";
        }
    #endif

    int err = Pa_Initialize();
    if (err != paNoError) {
        emit uiShowToast("ERROR: Audio processing library could not be loaded!");
    }
}

vector<PAInput> QMLCppBridge::getAudioList() {
    vector<PAInput> micOptions;
    Pa_Terminate();

    int err = Pa_Initialize();
    if (err != paNoError) {
        emit uiShowToast("ERROR: Audio processing library could not be loaded!");
        return micOptions;
    }

    int num_devices = Pa_GetDeviceCount();
    for (int i = 0; i < num_devices; i++) {
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo->maxInputChannels > 0) {
            string curr_name = deviceInfo->name;
            micOptions.push_back({curr_name, i});
        }
    }

    return micOptions;
}

PAInput QMLCppBridge::getPAInput(string device_name, bool select_default_first) {
    // find and select device_name from audio list
    // if select_default_first tag is set, select first if device_name
    // does not exist, else return -1
    vector<PAInput> micOptions = getAudioList();

    PAInput selected_input = {"", -1};
    for (auto &input : micOptions) {
        if (input.name.compare(device_name) == 0) {
            selected_input = input;
            break;
        } else if (selected_input.paIndex == -1 && select_default_first) {
            selected_input = input;
        }
    }

    return selected_input;
}

void QMLCppBridge::selectDefaultMic()
{
    if (selectedMic.compare("") == 0) {
        currentMic = getPAInput(defaultMic, true).name;
    } else {
        currentMic = selectedMic;
    }
}

void QMLCppBridge::selectDefaultCamera()
{
    if (SELECTED_CAMERA_INDEX == -1) {
        const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
        CAMERA_INDEX = 0;
        for (int i = 0; i < cameras.size(); i++){
            int compare = cameras[i].description().compare(qDefaultCamera);
            if (compare == 0) {
                CAMERA_INDEX = i;
                break;
            }
        }
    } else {
        CAMERA_INDEX = SELECTED_CAMERA_INDEX;
    }
}

void QMLCppBridge::closingApplication()
{
    if (logFile != NULL) {
        fclose(logFile);
    }
}

void QMLCppBridge::settingsOpened()
{
    selectDefaultCamera();
    selectDefaultMic();

    QStringList micOptions = {};
    vector<PAInput> audioList = getAudioList();
    for (auto &audio : audioList) {
        micOptions.append(QString::fromStdString(audio.name));
    }

    const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    QStringList cameraOptions = {};
    for (int i = 0; i < cameras.size(); i++){
        cameraOptions.append(cameras[i].description());
    }

    int device_index = getPAInput(currentMic, false).paIndex;

    if (device_index == -1) {
        emit uiShowToast("Device disconnected. Please reopen settings dialog and try again");
    } else {
        auto updateSamplesPtr = bind(&QMLCppBridge::updateSamples, this, _1);
        micThread = new MicThread(updateSamplesPtr, getPAInput(currentMic, false).paIndex);
        micThread->start();
    }

    emit uiSettingsOpened(micOptions, QString::fromStdString(currentMic), TRIGGER_DB, cameraOptions, CAMERA_INDEX, upDownDetection, minThreshold, maxThreshold);
}

void QMLCppBridge::settingsClosed()
{
    micThread->stop();
    delete micThread;
    micThread = NULL;
    emit uiSettingsClosed();
}

void QMLCppBridge::updateSamples(float dB)
{
    emit uiUpdateSamples(dB);
}

void QMLCppBridge::micChanged(QString newMic)
{
    selectedMic = newMic.toStdString();
    micThread->stop();
    delete micThread;
    auto updateSamplesPtr = bind(&QMLCppBridge::updateSamples, this, _1);
    micThread = new MicThread(updateSamplesPtr, getPAInput(selectedMic, false).paIndex);
    micThread->start();
}

void QMLCppBridge::micThresholdChanged(float newThreshold)
{
    TRIGGER_DB = newThreshold;
}

void QMLCppBridge::minThresholdChanged(float newThreshold)
{
    minThreshold = newThreshold;
}

void QMLCppBridge::maxThresholdChanged(float newThreshold)
{
    maxThreshold = newThreshold;
}

void QMLCppBridge::cameraChanged(int camera)
{
    SELECTED_CAMERA_INDEX = camera;
}

void QMLCppBridge::upDownDetectionChanged(bool upDownDetection)
{
    this->upDownDetection = upDownDetection;
}

void QMLCppBridge::stopRecording()
{
    if (calibThread != NULL) {
        calibThread->stop();
        calibThread = NULL;
        emit uiCalibrationEnded(false);
    }

    if (shootThread != NULL) {
        shootThread->stop();
        shootThread = NULL;
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
    selectDefaultCamera();
    selectDefaultMic();

    #ifndef QT_QML_DEBUG
        const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
        if ((selectedMic.compare("") == 0 && currentMic.compare(defaultMic) != 0) ||
            (SELECTED_CAMERA_INDEX == -1 && cameras[CAMERA_INDEX].description().compare(qDefaultCamera) != 0)) {
            // settings not changed through Settings Dialog
            // and default settings do not exist
            emit uiCameraMicError();
            return;
        }
    #endif

    if (calibThread == NULL && shootThread == NULL) {
        #ifdef QT_QML_DEBUG
            VideoCapture cap("/Users/msundarmsa/stasys/calibration.mp4");
        #else
            VideoCapture cap(CAMERA_INDEX);
        #endif

        auto calibrationFinishedPtr = bind(&QMLCppBridge::calibrationFinished, this, _1, _2, _3, _4);
        int mic_index = getPAInput(currentMic, false).paIndex;
        calibThread = new CalibrationThread(cap, mic_index, TRIGGER_DB, minThreshold, maxThreshold, calibrationFinishedPtr, logFile);
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
    selectDefaultCamera();
    selectDefaultMic();

    #ifndef QT_QML_DEBUG
        const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
        if ((selectedMic.compare("") == 0 && currentMic.compare(defaultMic) != 0) ||
            (SELECTED_CAMERA_INDEX == -1 && cameras[CAMERA_INDEX].description().compare(qDefaultCamera) != 0)) {
            // settings not changed through Settings Dialog
            // and default settings do not exist
            emit uiCameraMicError();
            return;
        }
    #endif

    if (calibThread == NULL && shootThread == NULL) {
        #ifdef QT_QML_DEBUG
            VideoCapture cap("/Users/msundarmsa/stasys/calibration.mp4");
            TRIGGER_DB = -1;
        #else
            VideoCapture cap(CAMERA_INDEX);
        #endif

        auto removePreviousCalibCirclePtr = bind(&QMLCppBridge::removePreviousCalibCircle, this);
        auto clearTracePtr = bind(&QMLCppBridge::clearTrace, this, _1);
        auto updateViewPtr = bind(&QMLCppBridge::updateView, this, _1);
        auto addToBeforeShotTracePtr = bind(&QMLCppBridge::addToBeforeShotTrace, this, _1);
        auto drawShotCirclePtr = bind(&QMLCppBridge::drawShotCircle, this, _1);
        auto addToAfterShotTracePtr = bind(&QMLCppBridge::addToAfterShotTrace, this, _1);
        ShootController controller = { removePreviousCalibCirclePtr, clearTracePtr, updateViewPtr, addToBeforeShotTracePtr, drawShotCirclePtr, addToAfterShotTracePtr };

        int mic_index = getPAInput(currentMic, false).paIndex;
        shootThread = new ShootThread(shots.size(), cap, mic_index, upDownDetection, TRIGGER_DB, minThreshold, maxThreshold, RATIO1, adjustmentVec, fineAdjustment, controller, logFile);
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
    if (calibThread != NULL) {
        calibThread->stop();
        calibThread = NULL;
        adjustmentVec = {x, y};
        RATIO1 = PISTOL_CIRCLE_SIZE / radius;
        emit uiCalibrationEnded(success);
    }
}

void QMLCppBridge::removePreviousCalibCircle() {
    emit uiRemovePreviousCalibCircle();
}

void QMLCppBridge::clearTrace(bool resetGroupIfNecessary) {
    emit uiClearTrace(resetGroupIfNecessary);
}

void QMLCppBridge::updateView(Shot* shot) {
    shots.push_back(shot);
    int sn = shot->getSn();
    double score = shot->getScore();
    double stab = shot->getStab();
    double desc = shot->getDesc();
    double aim = shot->getAim();
    const char *angle = toString(shot->getAngle());

    size_t frames = 0.5 * FPS;
    QList<double> xtList = {};
    QList<double> ytList = {};
    QList<double> tList = {};

    ShotTrace shotTrace = shot->getShotTrace();
    vector<TracePoint> beforeTrace = shotTrace.getBeforeShotTrace();
    TracePoint shotPoint = shotTrace.getShotPoint();
    vector<TracePoint> afterTrace = shotTrace.getAfterShotTrace();

    if (beforeTrace.size() > frames && afterTrace.size() > frames) {
        // only plot xt/yt graph if beforeTrace and afterTrace are valid
        for (int i = frames; i >= 1; i--) {
            int pos = beforeTrace.size() - i;
            xtList.append(beforeTrace[pos].point.x);
            ytList.append(beforeTrace[pos].point.y);
            tList.append((beforeTrace[pos].time - shotPoint.time) / 1000);
        }

        xtList.append(shotPoint.point.x);
        ytList.append(shotPoint.point.y);
        tList.append(0);

        for (size_t i = 0; i < frames; i++) {
            xtList.append(afterTrace[i].point.x);
            ytList.append(afterTrace[i].point.y);
            tList.append((afterTrace[i].time - shotPoint.time) / 1000);
        }
    }

    QVariant xt = QVariant::fromValue(xtList);
    QVariant yt = QVariant::fromValue(ytList);
    QVariant ts = QVariant::fromValue(tList);

    emit uiUpdateView(sn, score, stab, desc, aim, QString(angle), xt, yt, ts);
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
