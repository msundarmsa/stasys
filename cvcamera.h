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
    Q_PROPERTY(bool detectCircles READ detectCircles WRITE setDetectCircles)
    Q_PROPERTY(bool showThresholds READ showThresholds WRITE setShowThresholds)
    Q_PROPERTY(int minThreshold READ minThreshold WRITE setMinThreshold)
    Q_PROPERTY(int maxThreshold READ maxThreshold WRITE setMaxThreshold)

    Q_INTERFACES(QQmlParserStatus)

public:
    CVCamera(QObject* parent = nullptr);

    QAbstractVideoSurface* videoSurface() const;
    void setVideoSurface(QAbstractVideoSurface* videoSurface);

    int device() const;
    void setDevice(int device);

    bool detectCircles() const;
    void setDetectCircles(bool detectCircles);

    bool showThresholds() const;
    void setShowThresholds(bool showThresholds);

    int minThreshold() const;
    void setMinThreshold(int minThreshold);

    int maxThreshold() const;
    void setMaxThreshold(int maxThreshold);

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
    bool m_detectCircles = false;
    bool m_showThresholds = false;
    int m_minThreshold = 30;
    int m_maxThreshold = 80;
    cv::SimpleBlobDetector::Params params;
    cv::Ptr<cv::SimpleBlobDetector> detector;
};

#endif // CVCAMERA_H
