#pragma once

#include <QThread>

class VideoLoader : public QThread
{
    Q_OBJECT

public:
    VideoLoader(const QString& filePath, QObject *parent = nullptr);
    void run() override;

signals:
    void progress(int value);
    void finished(bool success, const std::vector<QImage>& frames, double frameRate, int totalFrames);

private:
    QString filePath;
};
