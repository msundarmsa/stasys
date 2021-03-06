#include "cvcamera.h"
#include <QAbstractVideoSurface>
#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QQuickItem>
#include <QScreen>
#include <QTimer>
#include <QVideoSurfaceFormat>
#include <vector>

using namespace cv;
using namespace std;

CVCamera::CVCamera(QObject* parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
{
    m_timer->setInterval(50);
    connect(m_timer, &QTimer::timeout, this, &CVCamera::presentFrame);

    params.minThreshold = 30;
    params.maxThreshold = 80;

    params.filterByArea = true;
    params.minArea = 450;

    params.filterByCircularity = true;
    params.minCircularity = 0.85;

    params.filterByInertia = true;
    params.minInertiaRatio = 0.85;

    detector = SimpleBlobDetector::create(params);
}

QAbstractVideoSurface* CVCamera::videoSurface() const
{
    return m_videoSurface;
}

void CVCamera::setVideoSurface(QAbstractVideoSurface* videoSurface)
{
    if (m_videoSurface == videoSurface)
        return;

    m_videoSurface = videoSurface;
    emit videoSurfaceChanged();

    if (m_completed)
        startSurface();
}

int CVCamera::device() const
{
    return m_device;
}

void CVCamera::setDevice(int device)
{
    this->m_device = device;
    if (device >= 0)
    {
        if (video.isOpened()) {
            video.release();
        }

        video.open(device);
        if (video.isOpened()){
            m_timer->start();
        }
    }
    else
    {
        // stop timer and camera capture
        m_timer->stop();
        if (video.isOpened()) {
            video.release();
        }
    }
}

bool CVCamera::detectCircles() const
{
    return m_detectCircles;
}

void CVCamera::setDetectCircles(bool detectCircles)
{
    this->m_detectCircles = detectCircles;
}

bool CVCamera::showThresholds() const
{
    return m_showThresholds;
}

void CVCamera::setShowThresholds(bool showThresholds)
{
    this->m_showThresholds = showThresholds;
}

int CVCamera::minThreshold() const
{
    return m_minThreshold;
}

void CVCamera::setMinThreshold(int minThreshold)
{
    params.minThreshold = minThreshold;
    detector = SimpleBlobDetector::create(params);
    this->m_minThreshold = minThreshold;
}

int CVCamera::maxThreshold() const
{
    return m_maxThreshold;
}

void CVCamera::setMaxThreshold(int maxThreshold)
{
    params.maxThreshold = maxThreshold;
    detector = SimpleBlobDetector::create(params);
    this->m_maxThreshold = maxThreshold;
}

void CVCamera::classBegin()
{
}

void CVCamera::componentComplete()
{
    m_parentItem = qobject_cast<QQuickItem*>(parent());

    connect(m_parentItem, &QQuickItem::widthChanged, this, &CVCamera::parentSizeChanged);
    connect(m_parentItem, &QQuickItem::heightChanged, this, &CVCamera::parentSizeChanged);
}

void CVCamera::parentSizeChanged()
{
    m_completed = true;
    if (m_parentItem->width() != 0)
        startSurface();
}

QImage CVCamera::grabFrame()
{
    int width = (int)m_width;
    int height =(int)m_height;
    QImage img(width, height, QImage::Format_BGR888);

    if (video.isOpened()){
        Mat frame;
        video >> frame;
        Mat grayFrame;

        if (m_detectCircles) {
            if (frame.channels() == 3) {
                cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
            } else {
                grayFrame = frame;
            }

            vector<KeyPoint> keypoints;

            GaussianBlur(grayFrame, grayFrame, cv::Size(9, 9), 0);
            detector->detect(grayFrame, keypoints);

            if (keypoints.size() >= 1) {
                for (auto &kp : keypoints) {
                    circle(frame, kp.pt, kp.size / 2, Scalar(0, 0, 255), FILLED);
                }
            }
        }

        if (m_showThresholds) {
            if (grayFrame.rows == 0 && grayFrame.cols == 0) {
                if (frame.channels() == 3) {
                    cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
                } else {
                    grayFrame = frame;
                }
            }

            Mat minBinImg;
            threshold(grayFrame, minBinImg, m_minThreshold, 255, cv::THRESH_BINARY);
            cvtColor(minBinImg, minBinImg, COLOR_GRAY2BGR);

            Mat maxBinImg;
            threshold(grayFrame, maxBinImg, m_maxThreshold, 255, cv::THRESH_BINARY);
            cvtColor(maxBinImg, maxBinImg, COLOR_GRAY2BGR);

            const Mat imgs[2] = { minBinImg, maxBinImg };
            hconcat(imgs, 2, frame);
        }

        img = QImage((uchar*) frame.data, frame.cols, frame.rows, frame.step, QImage::Format_BGR888);
    } else {
        QColor colors[3] = {QColor("red"), QColor("green"), QColor("blue")};
        for (int i = 0; i < width; i++){
            for (int j = 0; j < height; j++) {
                int block_x = i / (width / 3);
                int block_y = j / (height / 3);
                int z = (block_x + block_y) % 3;

                img.setPixelColor(i, j, colors[z]);
            }
        }
    }

    return img;
}

void CVCamera::startSurface()
{
    if (!m_videoSurface) {
        m_timer->stop();
        return;
    }

    m_width = m_parentItem->width();
    m_height = m_parentItem->height();
    QVideoSurfaceFormat format(QSizeF{ m_width, m_height  }.toSize(),
                               QVideoFrame::Format_ARGB32, QAbstractVideoBuffer::NoHandle);
    m_videoSurface->start(format);
}

void CVCamera::presentFrame()
{
    QImage image = grabFrame().convertToFormat(QImage::Format_ARGB32);
    QVideoFrame videoFrame(image);
    m_videoSurface->present(videoFrame);
}
