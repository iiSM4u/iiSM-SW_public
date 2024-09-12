#pragma once

#include <QWidget>
#include <QMutex>
#include <QThread>
#include <QButtonGroup>
#include <QTime>
#include <QProgressDialog>

#include <opencv2/opencv.hpp>
#include <miicam.h>

#include "simp_const_value.h"
#include "video_format_type.h"
#include "preset_contrast_curve.h"

namespace Ui {
class TabCamera;
}

class TabCamera : public QWidget
{
    Q_OBJECT

public:
    explicit TabCamera(QWidget *parent = nullptr);
    ~TabCamera();

    void onTabActivated();

protected:
    void closeEvent(QCloseEvent*) override;

signals:
    void evtCallback(unsigned nEvent);

private slots:
    void cbResoution_SelectedIndexChanged(int index);
    void cbFormat_SelectedIndexChanged(int index);
    void btnPlayCamera_Click();
    void btnStopCamera_Click();
    void btnCaptureCamera_Click();
    void btnRecordOn_Click();
    void btnRecordOption_Click();

    void sliderExposureTime_valueChanged(int value);
    void editExposureTime_editingFinished();
    void sliderGain_valueChanged(int value);
    void editGain_editingFinished();
    void sliderContrast_valueChanged(int value);
    void editContrast_editingFinished();
    void sliderGamma_valueChanged(int value);
    void editGamma_editingFinished();
    void btnCurveSetting_Click();
    void cbCurvePreset_SelectedIndexChanged(int index);
    void chkFlipHorizontal_CheckedChanged(Qt::CheckState checkState);
    void chkFlipVertical_CheckedChanged(Qt::CheckState checkState);
    void chkDarkFieldCorrection_CheckedChanged(Qt::CheckState checkState);
    void btnDarkFieldCorrection_Click();
    void btnGroupCooling_Click(int id);

    void sliderTemperature_valueChanged(int value);
    void editTemperature_editingFinished();
    void btnZoomIn_Click();
    void btnZoomOut_Click();

    void UpdatePreviewUI();
    void UpdateMousePosition(int x, int y, const QColor &color);
    void EnableUI(bool isPlay);

    void onVideoWritingProgress(int current, int total);
    void onVideoWritingCanceled();
    void onVideoWritingFinished(bool success);

private:
    Ui::TabCamera *ui;

    MiicamDeviceV2 miiDevice;
    HMiicam miiHcam = nullptr;

    QMutex imageMutex, contrastCurvesMutex;
    std::thread threadCamera;

    QButtonGroup *btnGroupCooling;
    QTimer *timerFPS, *timerVideoRecord;
    QProgressDialog *videoWritingDialog;

    uchar *rawCameraData = nullptr;
    QImage resultFrame;
    std::vector<cv::Mat> recordFrames;
    QImage::Format imageFormat = QImage::Format_RGB888;

    unsigned int imageWidth = 0;
    unsigned int imageHeight = 0;
    unsigned int rawCameraWidth = 0;
    unsigned int rawCameraHeight = 0;

    float zoomFactor = 1.0f;
    int resolutionIndex = 0;
    int cameraDelay = SimpConstValue::DELAY_CAMERA;

    QTime recordStartTime;
    VideoFormatType recordFormat = VideoFormatType::MJPEG;
    double recordFrameRate = SimpConstValue::RECORD_FRAME_RATE_DEFAULT;
    int recordQuality = SimpConstValue::RECORD_QUALITY_DEFAULT;
    int recordTimeLimit = 0;
    QString recordDir, captureDir; // 생성자에서 초기화 함

    bool isCameraOn = false, isCameraRun = false, isCameraPlay = false, isRecordOn = false, isDarkFieldCorrectCapture = false;
    bool isRawCameraData = false, isFlipHorizontal = false, isFlipVertical = false;

    bool isUpdateContrastCurve = false;
    std::vector<PresetContrastCurve> presetsContrastCurve;
    QVector<QPointF> contrastCurvePoints;

    void ConnectUI();
    void InitUI();
    void UpdatePreview();
    void StartRecord();
    void FinishRecord();

    void LoadPresets();
    void UpdatePresetContrastCurve(const std::vector<PresetContrastCurve>& presets, const int index = 0);
    void UpdateContrastCurvePoints(const QVector<QPointF>& points);

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
