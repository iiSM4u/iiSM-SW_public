#include <QThread>
#include <opencv2/opencv.hpp>
#include "video_format_type.h"

class WorkerVideoWritingCV : public QThread
{
    Q_OBJECT

public:
    WorkerVideoWritingCV(
        const std::vector<cv::Mat>& frames
        , const VideoFormatType format
        , const int recordSecond
        , const int quality
        , const QString& filePath
        , QObject *parent = nullptr
    );
    void run() override;

signals:
    void progress(int current, int total);
    void finished(bool success);
    void cancelled();

private:
    std::vector<cv::Mat> frames;
    VideoFormatType format;
    QString filePath;
    int recordSecond, quality;
};
