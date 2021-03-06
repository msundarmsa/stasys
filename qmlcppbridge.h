#ifndef QMLCPPBRIDGE_H
#define QMLCPPBRIDGE_H

#include <QObject>
#include <QDebug>
#include "Utils.h"
#include "CalibrationThread.h"
#include "ShootThread.h"
#include "MicThread.h"

typedef struct PAInput {
    string name;
    int paIndex;
} PAInput;

class QMLCppBridge : public QObject
{
    Q_OBJECT
public:
    explicit QMLCppBridge(QObject *parent = nullptr);
    void calibrationFinished(bool success, double x, double y, double radius);
    void removePreviousCalibCircle();
    void clearTrace(bool resetGroupIfNecessary);
    void updateView(Shot* shot);
    void addToBeforeShotTrace(Vector2D center);
    void drawShotCircle(Vector2D center);
    void addToAfterShotTrace(Vector2D center);
    void updateSamples(float dB);
    void selectDefaultCamera();
    void selectDefaultMic();
    PAInput getPAInput(string device_name, bool select_default_first);
    vector<PAInput> getAudioList();

    Q_INVOKABLE void calibrationClicked();
    Q_INVOKABLE void shootClicked();
    Q_INVOKABLE void settingsOpened();
    Q_INVOKABLE void settingsClosed();
    Q_INVOKABLE void micChanged(QString mic);
    Q_INVOKABLE void micThresholdChanged(float newThreshold);
    Q_INVOKABLE void cameraChanged(int camera);
    Q_INVOKABLE void upDownDetectionChanged(bool upDownDetection);
    Q_INVOKABLE void stopRecording();
    Q_INVOKABLE void adjustCalibration(double deltaX, double deltaY);
    Q_INVOKABLE void closingApplication();
    Q_INVOKABLE void minThresholdChanged(float newThreshold);
    Q_INVOKABLE void maxThresholdChanged(float newThreshold);

private:
    FILE* logFile = stdout;
    CalibrationThread* calibThread = NULL;
    ShootThread* shootThread = NULL;
    MicThread *micThread = NULL;
    std::string currentMic = "";
    std::string selectedMic = "";
    float micThreshold = 0.0;
    int CAMERA_INDEX = -1;
    int SELECTED_CAMERA_INDEX = -1;
    float TRIGGER_DB = 45.0;
    double RATIO1 = 1; // px from camera to mm
    Vector2D adjustmentVec = {0, 0};
    Vector2D fineAdjustment = {0, 0};
    bool upDownDetection = true;
    std::vector<Shot*> shots;
    const string defaultMic = "Realtek USB2.0 Mic";
    const QString qDefaultCamera = "USB Camera";
    int minThreshold = 30;
    int maxThreshold = 80;

signals:
    void uiCalibrationStarted();
    void uiCalibrationEnded(bool success);
    void uiShootingStarted();
    void uiShootingEnded();
    void uiShowToast(QString message);
    void uiRemovePreviousCalibCircle();
    void uiClearTrace(bool resetGroupIfNecessary);
    void uiUpdateView(int sn, double score, double stab, double desc, double aim, QString angle, QVariant xt, QVariant yt, QVariant ts);
    void uiAddToBeforeShotTrace(double x, double y);
    void uiDrawShotCircle(double x, double y);
    void uiAddToAfterShotTrace(double x, double y);
    void uiSettingsOpened(QStringList micOptions, QString defaultMic, double TRIGGER_DB, QStringList cameraOptions, int defaultCamera, bool upDownDetection, double minThreshold, double maxThreshold);
    void uiSettingsClosed();
    void uiUpdateSamples(float dB);
    void uiCameraMicError();
};

#endif // QMLCPPBRIDGE_H
