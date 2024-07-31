#include "videoloader.h"
#include <QImage>
#include <opencv2/opencv.hpp>

VideoLoader::VideoLoader(const QString& filePath, QObject *parent)
    : QThread(parent), filePath(filePath)
{
}

void VideoLoader::run()
{
    cv::VideoCapture video(filePath.toStdString());
    if (!video.isOpened())
    {
        emit finished(false, {}, 0, 0);
        return;
    }

    double frameRate = video.get(cv::CAP_PROP_FPS);
    int totalFrames = static_cast<int>(video.get(cv::CAP_PROP_FRAME_COUNT));
    std::vector<QImage> frames;
    frames.reserve(totalFrames);

    for (int i = 0; i < totalFrames; ++i)
    {
        cv::Mat frame;
        if (video.read(frame))
        {
            QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
            frames.emplace_back(img.rgbSwapped()); // Convert BGR to RGB
        }
        emit progress(((i + 1) * 100) / totalFrames); // Ensure progress goes up to 100%
    }

    emit finished(true, frames, frameRate, totalFrames);
}
