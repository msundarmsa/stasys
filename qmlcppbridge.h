#ifndef QMLCPPBRIDGE_H
#define QMLCPPBRIDGE_H

#include <QObject>
#include <QDebug>

class QMLCppBridge : public QObject
{
    Q_OBJECT
public:
    explicit QMLCppBridge(QObject *parent = nullptr);
    void calibrationFinished(bool success, double x, double y, double radius, int frameWidth, int frameHeight);
    void testCallback();

    Q_INVOKABLE void calibrationClicked();
    Q_INVOKABLE void shootClicked();

signals:
    void calibrationCompleted(bool success);
};

#endif // QMLCPPBRIDGE_H
