#ifndef QMLCPPBRIDGE_H
#define QMLCPPBRIDGE_H

#include <QObject>
#include <QDebug>
#include "Utils.h"

class QMLCppBridge : public QObject
{
    Q_OBJECT
public:
    explicit QMLCppBridge(QObject *parent = nullptr);
    void calibrationFinished(bool success, double x, double y, double radius, int frameWidth, int frameHeight);
    void removePreviousCalibCircle();
    void clearTrace(bool resetGroupIfNecessary);
    void updateView(Shot* shot);
    void addToBeforeShotTrace(Vector2D center);
    void drawShotCircle(Vector2D center);
    void addToAfterShotTrace(Vector2D);

    Q_INVOKABLE void calibrationClicked();
    Q_INVOKABLE void shootClicked();
private:
    FILE* logFile = NULL;
    Vector2D adjustmentVec = {0, 0};
    double radius = 0;

signals:
    void calibrationCompleted(bool success);
    void uiRemovePreviousCalibCircle();
    void uiClearTrace(bool resetGroupIfNecessary);
    void uiUpdateView(double x, double y, double score, double stab, double desc, double aim);
    void uiAddToBeforeShotTrace(double x, double y);
    void uiDrawShotCircle(double x, double y);
    void uiAddToAfterShotTrace(double x, double y);

};

#endif // QMLCPPBRIDGE_H
