#include "worker_video_writing_cv.h"
#include "simp_util.h"

WorkerVideoWritingCV::WorkerVideoWritingCV(
    const std::vector<cv::Mat>& frames
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

void WorkerVideoWritingCV::run()
{
    if (frames.size() == 0 || filePath.isEmpty())
    {
        emit finished(false);
        return;
    }

    int totalFrames = frames.size();
    int fourcc = SimpUtil::getVideoFourcc(format);
    double frameRate = frames.size() / (double)recordSecond;

    // Create VideoWriter object
    cv::VideoWriter writer(filePath.toStdString(), fourcc, frameRate, cv::Size(frames[0].cols, frames[0].rows));

    // quality는 특정 format에만 적용된다.
    if (format == VideoFormatType::MJPEG)
    {
        writer.set(cv::VIDEOWRITER_PROP_QUALITY, quality);
    }

    // 긴 작업이 시작되기 전에 스레드가 중단 요청을 받았는지 확인
    if (QThread::currentThread()->isInterruptionRequested()) {
        emit cancelled();
        return;
    }

    // Write frames to video file
    int size = frames.size();
    for (int i = 0; i < size ; i++)
    {
        writer.write(frames[i]);

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
