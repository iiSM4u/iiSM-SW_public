#pragma once

#include <QThread>

class WorkerVideoLoading : public QThread
{
    Q_OBJECT

public:
    WorkerVideoLoading(const QString& filePath, QObject *parent = nullptr);
    void run() override;

signals:
    void progress(int current, int total);
    void finished(bool success, const std::vector<QImage>& frames, double frameRate, int totalFrames);
    void cancelled();

private:
    QString filePath;
};
