#pragma once

#include <QMainWindow>
#include <QMediaPlayer>
#include <QFileSystemModel>
#include <QTimer>
#include <QGraphicsScene>
#include <QThread>
#include <QMutex>

#include <opencv2/opencv.hpp>
#include <miicam.h>

#include "constants.h"

// C 라이브러리를 참조할 떄는 extern C로 묶는 것이 링킹 문제를 방지하는데 도움이 됨
extern "C" {
#include <glib.h>
#include <gegl.h>
#include <gegl-0.4/gegl.h>
#include <gegl-0.4/gegl-init.h>
#include <gegl-0.4/gegl-node.h>
#include <gegl-0.4/gegl-buffer.h>
}

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
    void resizeEvent(QResizeEvent *event) override;

signals:
    void evtCallback(unsigned nEvent);

private slots:
    // ui events

    // preview
    void cbResoution_SelectedIndexChanged(int index);
    void cbFormat_SelectedIndexChanged(int index);

    void chkRecord_CheckedChanged(Qt::CheckState checkState);

    void btnPlayCamera_Click();
    void btnStopCamera_Click();
    void btnCaptureCamera_Click();
    void btnRecordOption_Click();

    void btnZoomIn_Click();
    void btnZoomOut_Click();


    void btnBrightnessContrast_Click();
    void btnStress_Click();
    void btnStretchContrast_Click();

    // video
    void btnLoadVideo_Click();
    void btnPlayVideo_Click();
    void btnStopVideo_Click();

    void onVideoStatusChanged(QMediaPlayer::MediaStatus status);

    // frame
    void btnLoadFrame_Click();

    void onSelecteImage(const QModelIndex &index);

    void UpdateGraphicsView();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scenePreview, *sceneFrame;
    QGraphicsPixmapItem *pmiPreview = nullptr, *pmiFrame = nullptr;

    QMediaPlayer *mpVideoFile;
    QFileSystemModel *modelFrames;
    QTimer* timer;

    MiicamDeviceV2 miiDevice;
    HMiicam miiHcam = nullptr;

    uchar *rawCameraData = nullptr;
    QImage resultImage;
    std::vector<cv::Mat> videoFrames;

    unsigned imageWidth = 0;
    unsigned imageHeight = 0;
    unsigned rawCameraWidth = 0;
    unsigned rawCameraHeight = 0;
    int resolutionIndex = 0;
    int recordFormat = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
    QString recordFormatExtension = "avi";

    //void resizeEvent(QResizeEvent* event) override;
    bool isOn = false;
    bool isCameraRun = false;
    bool isCameraPlay = false;
    bool isVideoPlay = false;
    bool isRecordOn = false;

    bool isUpdateBrightnessContrast = false;
    bool isUpdateStress = false;
    bool isUpdateStretchContrast = false;
    bool isUpdateContrastCurve = false;

    float zoomFactor = 1.0f;

    double brightness = DEFAULT_BRIGHTNESS, contrast = DEFAULT_CONTRAST;
    int stressRadius = DEFAULT_STRESS_RADIUS, stressSamples = DEFAULT_STRESS_SAMPLES, stressIterations = DEFAULT_STRESS_INTERATIONS;
    bool stressEnhanceShadows = DEFAULT_STRESS_ENHANCE_SHADOWS;
    bool stretchContrastKeepColors = DEFAULT_STRETCH_CONTRAST_KEEP_COLORS, stretchContrastPerceptual = DEFAULT_STRETCH_CONTRAST_PERCEPTUAL;
    int contrastCurveSamplingPoints = DEFAULT_CONTRAST_CURVE_SAMPLING_PLINTS;
    GeglCurve *contrastCurves = nullptr;

    QString captureDir = "";

    QMutex imageMutex;


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
    void UpdateBrightnessContrast(QImage& source, const double brightness, const double contrast);
    void UpdateStress(QImage& source, const int radius, const int samples, const int iterations, const bool enhanceShadows);
    void UpdateStretchContrast(QImage& source, const bool keepColors, const bool perceptual);
    void UpdateContrastCurve(QImage& source, const GeglCurve* curve, const int samplingPoints);


    // custom
    void SetupModel(const QString& capturePath);
    void SetPlayVideo(bool value);
    void UpdatePreview();


    std::thread updateThread;
};
