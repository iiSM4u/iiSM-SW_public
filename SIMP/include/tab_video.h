#pragma once

#include <QWidget>
#include <QThread>
#include <QFileSystemModel>
#include <QProgressDialog>

#include "simp_const_value.h"
#include "dialog_image_processing.h"

namespace Ui {
class TabVideo;
}

class TabVideo : public QWidget
{
    Q_OBJECT

public:
    explicit TabVideo(QWidget *parent = nullptr);
    ~TabVideo();

    void onTabActivated();

protected:
    void closeEvent(QCloseEvent*) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void UpdateMousePosition(int x, int y, const QColor& color);
    void UpdateVideoUI();
    void EnableUI(bool enable);

    void lvVideo_Click(const QModelIndex& index);
    void btnOpenDir_Click();
    void btnZoomIn_Click();
    void btnZoomOut_Click();
    void btnVideoProcessing_Click();
    void btnVideoSave_Click();

    void btnPlayVideo_Click();
    void btnStopVideo_Click();
    void btnSaveFrame_Click();

    void sliderVideo_sliderMoved(int position);

    void onVideoLoadingProgress(int current, int total);
    void onVideoLoadingCanceled();
    void onVideoLoadingFinished(bool success, const std::vector<QImage>& frames, double frameRate, int totalFrames);

    void onVideoConvertingProgress(int current, int total);
    void onVideoConvertingCanceled();
    void onVideoConvertingFinished(bool success, const std::vector<QImage>& frames);

    void onVideoWritingProgress(int current, int total);
    void onVideoWritingCanceled();
    void onVideoWritingFinished(bool success);


private:
    Ui::TabVideo *ui;

    std::thread threadVideo;

    QProgressDialog *loadingDialog, *processingDialog, *videoWritingDialog;
    QFileSystemModel *filesystemModel;
    DialogImageProcessing *dialogImageProcessing;

    QModelIndex currentVideoIndex;
    QString recordDir;

    int videoDelay = SimpConstValue::DELAY_CAMERA;
    float zoomFactor = 1.0f;
    int lastPresetIndex = -1;
    bool isOn = false, isVideoPlay = false;

    int videoTotalFrame, currentFrameIndex;
    double videoFrameRates;

    QImage currentFrame;
    std::vector<QImage> videoFrames;

    void ConnectUI();
    void InitUI();
    void UpdateVideo();
    void ProcessingVideo();
};
