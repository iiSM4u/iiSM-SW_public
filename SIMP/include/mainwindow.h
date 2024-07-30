#pragma once

#include <QMainWindow>
#include <QMediaPlayer>
#include <QFileSystemModel>
#include <QTimer>
#include <QGraphicsScene>
#include <QThread>
#include <QMutex>
#include <QButtonGroup>

#include <opencv2/opencv.hpp>
#include <miicam.h>

#include "constants.h"
#include "video_format_type.h"

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
    void ConnectUI();
    void InitUI();
    void EnablePreviewUI(bool isPlay);
    void EnableDarkfieldUI(bool value);
    void EnableCoolingUI(bool value);

    /////////////////////// preview
    // thread
    void UpdateGraphicsView();

    // custom
    void UpdatePreviewMousePosition(int x, int y, const QColor &color);
    void UpdateVideoMousePosition(int x, int y, const QColor &color);
    void UpdateFrameMousePosition(int x, int y, const QColor &color);

    void cbResoution_SelectedIndexChanged(int index);
    void cbFormat_SelectedIndexChanged(int index);

    void btnPlayCamera_Click();
    void btnStopCamera_Click();
    void btnCaptureCamera_Click();

    void chkRecord_CheckedChanged(Qt::CheckState checkState);
    void btnRecordOption_Click();

    void sliderExposureTime_sliderMoved(int position);
    void editExposureTime_editingFinished();

    void sliderGain_sliderMoved(int position);
    void editGain_editingFinished();

    void sliderContrast_sliderMoved(int position);
    void editContrast_editingFinished();

    void sliderGamma_sliderMoved(int position);
    void editGamma_editingFinished();

    void btnCurveSetting_Click();
    void cbCurvePreset_SelectedIndexChanged(int index);

    void chkDarkfield_CheckedChanged(Qt::CheckState checkState);
    void btnDarkfieldCapture_Click();
    void editDarkfieldQuantity_editingFinished();

    void btnGroupCooling_Click(int id);

    void sliderTemperature_sliderMoved(int position);
    void editTemperature_editingFinished();


    void btnZoomIn_Click();
    void btnZoomOut_Click();

    void btnBrightnessContrast_Click();
    void btnStress_Click();
    void btnStretchContrast_Click();


    /////////////////////// video
    void btnLoadVideo_Click();
    void btnPlayVideo_Click();
    void btnStopVideo_Click();

    void onVideoStatusChanged(QMediaPlayer::MediaStatus status);

    /////////////////////// frame
    void btnLoadFrame_Click();
    void lvFrames_Click(const QModelIndex &index);


private:
    Ui::MainWindow *ui;

    //QGraphicsScene *sceneFrame;
    //QGraphicsPixmapItem *pmiFrame = nullptr;
    QButtonGroup *btnGroupCooling;

    QMediaPlayer *mpVideoFile;
    QFileSystemModel *modelFrames;
    QTimer *timerFPS, *timerVideoRecord;
    QTime recordStartTime;

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
    VideoFormatType recordFormat = VideoFormatType::MJPEG;
    double recordFrameRate = 30.0;

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

    double gegl_brightness = GEGL_BRIGHTNESS_DEFAULT, gegl_contrast = GEGL_CONTRAST_DEFAULT;
    int gegl_stress_radius = GEGL_STRESS_RADIUS_DEFAULT, gegl_stress_samples = GEGL_STRESS_SAMPLES_DEFAULT, gegl_stress_iterations = GEGL_STRESS_ITERATIONS_DEFAULT;
    bool gegl_stress_enhance_shadows = GEGL_STRESS_ENHANCE_SHADOWS_DEFAULT;
    bool gegl_stretch_contrast_keep_colors = GEGL_STRETCH_CONTRAST_KEEP_COLORS_DEFAULT, gegl_stretch_contrast_perceptual = GEGL_STRETCH_CONTRAST_PERCEPTUAL_DEFAULT;
    int gegl_contrast_curve_sampling_points = GEGL_CONTRAST_CURVE_SAMPLING_POINTS_DEFAULT;
    GeglCurve *contrastCurves = nullptr;

    QString captureDir = "";

    QMutex imageMutex;

    void onTimerFpsCallback();

    //CustomPlainTextEdit* ChangeQPlainTextEditToCustom(QPlainTextEdit* source);

    // mii camera
    void FindCamera();
    void OpenCamera();
    void CloseCamera();
    void StartCamera();

    void InitCameraResolution();
    void UpdateExposureTime();
    void UpdateSensorTemperature();

    void onMiiCameraCallback(unsigned nEvent);
    void handleImageEvent();
    void handleExpoEvent();
    void handleTempTintEvent();
    void handleStillImageEvent();
    static void __stdcall eventCallBack(unsigned nEvent, void* pCallbackCtx);

    // gegl
    void InitGegl();
    void CloseGegl();
    void UpdateGeglContrast(
        QImage& source
        , const double brightness
        , const double contrast
        , const int radius
        , const int samples
        , const int iterations
        , const bool enhanceShadows
        , const bool keepColors
        , const bool perceptual
        , const GeglCurve* curve
        , const int samplingPoints
        , const bool isUpdateBrightnessContrast
        , const bool isUpdateStress
        , const bool isUpdateStretchContrast
        , const bool isUpdateContrastCurve
    );

    // custom
    void SetPlayVideo(bool value);
    void UpdatePreview();    
    void RecordVideo(std::vector<cv::Mat>& videoFrames, const VideoFormatType format, const double frameRate, const int quality);


    std::thread updateThread;
};
