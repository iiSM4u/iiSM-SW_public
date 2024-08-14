#pragma once

#include <QWidget>
#include <QMutex>
#include <QThread>
#include <QButtonGroup>
#include <QTime>

#include <opencv2/opencv.hpp>
#include <miicam.h>

#include "simp_const_value.h"
#include "video_format_type.h"

namespace Ui {
class TabCamera;
}

class TabCamera : public QWidget
{
    Q_OBJECT

public:
    explicit TabCamera(QWidget *parent = nullptr);
    ~TabCamera();

signals:
    void evtCallback(unsigned nEvent);

private:
    Ui::TabCamera *ui;

    MiicamDeviceV2 miiDevice;
    HMiicam miiHcam = nullptr;

    QMutex imageMutex, contrastCurvesMutex;
    QThread threadCamera;

    QButtonGroup *btnGroupCooling;
    QTimer *timerFPS, *timerVideoRecord;

    uchar *rawCameraData = nullptr;
    QImage resultPreview;
    std::vector<cv::Mat> recordFrames;
    QImage::Format imageFormat = QImage::Format_RGB888;

    unsigned int imageWidth = 0;
    unsigned int imageHeight = 0;
    unsigned int rawCameraWidth = 0;
    unsigned int rawCameraHeight = 0;

    float zoomFactor = 1.0f;
    int resolutionIndex = 0;

    QTime recordStartTime;
    VideoFormatType recordFormat = VideoFormatType::MJPEG;
    double recordFrameRate = SimpConstValue::RECORD_FRAME_RATE_DEFAULT;
    int recordQuality = SimpConstValue::RECORD_QUALITY_DEFAULT;
    int recordTimeLimit = 0;
    QString recordDir, captureDir; // 생성자에서 초기화 함

    bool isCameraOn = false, isCameraRun = false, isCameraPlay = false, isRecordOn = false, isDarkFieldCorrectCapture = false;

    void StartRecord();
    void FinishRecord();

    // mii camera
    void FindCamera();
    void OpenCamera();
    void CloseCamera();
    void StartCamera();

    void InitCameraResolution();
    void UpdateExposureTime();
    void InitSensorTemperature();

    void onMiiCameraCallback(unsigned nEvent);
    void onTimerFpsCallback();

    void handleImageEvent();
    static void __stdcall eventCallBack(unsigned nEvent, void* pCallbackCtx);
};
