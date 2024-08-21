#include "worker_video_writing_qimage.h"
#include "simp_util.h"

#include <opencv2/opencv.hpp>

WorkerVideoWritingQImage::WorkerVideoWritingQImage(
    const std::vector<QImage>& frames
    , const VideoFormatType format
    , const int recordSecond
    , const int quality
    , const QString& filePath
    , QObject *parent
    ) :
    frames(frames)
    , format(format)
    , recordSecond(recordSecond)
    , quality(quality)
    , filePath(filePath)
    , QThread(parent)
{}

void WorkerVideoWritingQImage::run()
{
    if (this->frames.size() == 0 || this->filePath.isEmpty())
    {
        emit finished(false);
        return;
    }

    int totalFrames = this->frames.size();
    int fourcc = SimpUtil::getVideoFourcc(this->format);
    double frameRate = this->frames.size() / (double)this->recordSecond;

    // Create VideoWriter object
    int width = this->frames[0].width();
    int height = this->frames[0].height();
    cv::VideoWriter writer(this->filePath.toStdString(), fourcc, frameRate, cv::Size(width, height));

    // quality는 특정 format에만 적용된다.
    if (this->format == VideoFormatType::MJPEG)
    {
        writer.set(cv::VIDEOWRITER_PROP_QUALITY, this->quality);
    }

    // 긴 작업이 시작되기 전에 스레드가 중단 요청을 받았는지 확인
    if (QThread::currentThread()->isInterruptionRequested()) {
        emit cancelled();
        return;
    }

    // Write frames to video file
    int size = this->frames.size();
    for (int i = 0; i < size ; i++)
    {
        QImage& frame = this->frames[i];
        cv::Mat mat(height, width, CV_8UC3, const_cast<uchar*>(frame.bits()), frame.bytesPerLine());
        cv::Mat matBGR;
        cv::cvtColor(mat, matBGR, cv::COLOR_RGB2BGR);  // rgb -> bgr

        writer.write(matBGR);

        if (QThread::currentThread()->isInterruptionRequested()) {
            writer.release();
            emit cancelled();
            return;
        }

        emit progress(i, totalFrames); // Ensure progress goes up to 100%
    }

    // Release the VideoWriter
    writer.release();

    emit finished(true);
}
