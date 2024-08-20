#pragma once

#include <QWidget>
#include <QThread>
#include <QFileSystemModel>
#include <QProgressDialog>

namespace Ui {
class TabVideo;
}

class TabVideo : public QWidget
{
    Q_OBJECT

public:
    explicit TabVideo(QWidget *parent = nullptr);
    ~TabVideo();

protected:
    void closeEvent(QCloseEvent*) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void UpdateMousePosition(int x, int y, const QColor& color);
    void UpdateVideoUI();

    void lvVideo_Click(const QModelIndex& index);
    void btnOpenDir_Click();
    void btnZoomIn_Click();
    void btnZoomOut_Click();
    void btnVideoProcessing_Click();

    void btnPlayVideo_Click();
    void btnStopVideo_Click();

    void sliderVideo_sliderMoved(int position);

    void onVideoLoadingProgress(int value);
    void onVideoLoadingFinished(bool success, const std::vector<QImage>& frames, double frameRate, int totalFrames);

private:
    Ui::TabVideo *ui;

    std::thread threadVideo;

    QProgressDialog *progressDialog;
    QFileSystemModel *filesystemModel;
    QString recordDir; // 생성자에서 초기화 함

    float zoomFactor = 1.0f;
    bool isOn = false, isVideoPlay = false;

    int videoTotalFrame;
    double videoFrameRates;
    int currentFrame;

    QImage resultVideo;
    std::vector<QImage> videoFrames;

    void ConnectUI();
    void InitUI();
    void UpdateVideo();
};
