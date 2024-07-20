#pragma once

#include <QMainWindow>
#include <QMediaPlayer>
#include <QFileSystemModel>
#include <QTimer>
#include <QGraphicsScene>

#include <opencv2/opencv.hpp>
#include <miicam.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent*) override;

signals:
    void evtCallback(unsigned nEvent);

private slots:
    // ui events

    // preview
    void cbResoution_SelectedIndexChanged(int index);
    void cbFormat_SelectedIndexChanged(int index);

    void chkRecord_CheckedChanged();

    void btnPlayCamera_Click();
    void btnStopCamera_Click();
    void btnCaptureCamera_Click();
    void btnRecordOption_Click();

    void btnZoomIn_Click();
    void btnZoomOut_Click();

    // ui thread
    void updatePreview(QImage outputImage);
    void updateFrame(QImage outputImage);


    // video
    void btnLoadVideo_Click();
    void btnPlayVideo_Click();
    void btnStopVideo_Click();

    void onVideoStatusChanged(QMediaPlayer::MediaStatus status);



    // frame
    void btnLoadFrame_Click();

    void onSelecteImage(const QModelIndex &index);


    // common
    void onTimerCallback();


    // mii camera
    void FindCamera();
    void OpenCamera();
    void CloseCamera();
    void StartCamera();

    void onMiiCameraCallback(unsigned nEvent);
    void handleImageEvent();
    void handleExpoEvent();
    void handleTempTintEvent();
    void handleStillImageEvent();
    static void __stdcall eventCallBack(unsigned nEvent, void* pCallbackCtx);


    // gegl
    void InitGegl();
    void CloseGegl();
    void btnBrightnessContrast_Click();
    void btnStress_Click();
    void btnStretchContrast_Click();



    // custom
    void SetupModel(const QString& capturePath);
    void SetPlayVideo(bool value);


private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;

    QMediaPlayer *mpVideoFile;
    QFileSystemModel *modelFrames;
    QTimer* timer;
    //QPixmap currentPixmap;

    MiicamDeviceV2 miiDevice;
    HMiicam miiHcam = nullptr;

    std::vector<cv::Mat> videoFrames;
    uchar* imageData = nullptr;
    int resolutionIndex = 0;
    unsigned imageWidth = 0;
    unsigned imageHeight = 0;

    //void resizeEvent(QResizeEvent* event) override;

    bool isCameraRun = false;
    bool isCameraPlay = false;
    bool isVideoPlay = false;

    float zoomFactor = 1.0f;

    QString captureDir = "";
};
