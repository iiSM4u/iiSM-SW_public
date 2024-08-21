#include <QThread>
#include <QImage>

#include "video_format_type.h"

class WorkerVideoWritingQImage : public QThread
{
    Q_OBJECT

public:
    WorkerVideoWritingQImage(
        const std::vector<QImage>& frames
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
    std::vector<QImage> frames;
    VideoFormatType format;
    QString filePath;
    int recordSecond, quality;
};
