#ifndef CVCAMERA_H
#define CVCAMERA_H

#include <QObject>
#include <QQmlParserStatus>
#include <opencv2/opencv.hpp>
#include <thread>

class QDesktopWidget;
class QQuickItem;
class QAbstractVideoSurface;
class QTimer;
class QScreen;

class CVCamera : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QAbstractVideoSurface* videoSurface READ videoSurface WRITE setVideoSurface NOTIFY videoSurfaceChanged)
    Q_PROPERTY(int device READ device WRITE setDevice)

    Q_INTERFACES(QQmlParserStatus)

public:
    CVCamera(QObject* parent = nullptr);

    QAbstractVideoSurface* videoSurface() const;
    void setVideoSurface(QAbstractVideoSurface* videoSurface);

    int device() const;
    void setDevice(int device);

    void classBegin() override;
    void componentComplete() override;

signals:
    void videoSurfaceChanged();

private:
    void parentSizeChanged();
    QImage grabFrame();
    void startSurface();
    void presentFrame();
    void run(int device);

    QAbstractVideoSurface* m_videoSurface = nullptr;
    QQuickItem* m_parentItem = nullptr;
    QTimer* m_timer;
    int m_device = -1;
    cv::VideoCapture video;
    cv::Mat curr_frame;
    qreal m_width = -1;
    qreal m_height = -1;
    std::thread *cameraThread;
    bool m_completed = false;
};

#endif // CVCAMERA_H
