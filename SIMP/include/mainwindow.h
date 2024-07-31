#pragma once

#include <QMainWindow>
#include <QMediaPlayer>
#include <QFileSystemModel>
#include <QTimer>
#include <QGraphicsScene>
#include <QThread>
#include <QMutex>
#include <QButtonGroup>
#include <QProgressDialog>

#include <opencv2/opencv.hpp>
#include <miicam.h>

#include "constants.h"
#include "video_format_type.h"
#include "videoloader.h"

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
    void UpdatePreviewUI();
    void UpdateVideoUI();


    // custom
    void UpdatePreviewMousePosition(int x, int y, const QColor& color);
    void UpdateVideoMousePosition(int x, int y, const QColor& color);
    void UpdateFrameMousePosition(int x, int y, const QColor& color);

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
    void lvVideo_Click(const QModelIndex& index);

    void sliderVideo_sliderMoved(int position);

    void onVideoLoadingProgress(int value);
    void onVideoLoadingFinished(bool success, const std::vector<QImage>& frames, double frameRate, int totalFrames);


    /////////////////////// frame
    void btnLoadFrame_Click();
    void lvFrame_Click(const QModelIndex& index);


private:
    Ui::MainWindow *ui;
    QProgressDialog *progressDialog;

    QButtonGroup *btnGroupCooling;

    //QMediaPlayer *mediaVideo;
    QFileSystemModel *filesystemVideo, *filesystemFrame;
    QTimer *timerFPS, *timerVideoRecord;

    MiicamDeviceV2 miiDevice;
    HMiicam miiHcam = nullptr;

    uchar *rawCameraData = nullptr;
    QImage resultPreview, resultVideo;
    std::vector<cv::Mat> recordFrames;

    unsigned int imageWidth = 0;
    unsigned int imageHeight = 0;
    unsigned int rawCameraWidth = 0;
    unsigned int rawCameraHeight = 0;

    float zoomFactor = 1.0f;

    int resolutionIndex = 0;

    QTime recordStartTime;
    VideoFormatType recordFormat = VideoFormatType::MJPEG;
    double recordFrameRate = RECORD_FRAME_RATE_DEFAULT;
    int recordQuality = RECORD_QUALITY_DEFAULT;
    int recordTimeLimit = 0;
    QString recordDir, captureDir; // 생성자에서 초기화 함

    int videoTotalFrame;
    double videoFrameRates;
    int currentFrame;
    std::vector<QImage> videoFrames;

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

    double gegl_brightness = GEGL_BRIGHTNESS_DEFAULT, gegl_contrast = GEGL_CONTRAST_DEFAULT;
    int gegl_stress_radius = GEGL_STRESS_RADIUS_DEFAULT, gegl_stress_samples = GEGL_STRESS_SAMPLES_DEFAULT, gegl_stress_iterations = GEGL_STRESS_ITERATIONS_DEFAULT;
    bool gegl_stress_enhance_shadows = GEGL_STRESS_ENHANCE_SHADOWS_DEFAULT;
    bool gegl_stretch_contrast_keep_colors = GEGL_STRETCH_CONTRAST_KEEP_COLORS_DEFAULT, gegl_stretch_contrast_perceptual = GEGL_STRETCH_CONTRAST_PERCEPTUAL_DEFAULT;
    int gegl_contrast_curve_sampling_points = GEGL_CONTRAST_CURVE_SAMPLING_POINTS_DEFAULT;
    GeglCurve *contrastCurves = nullptr;

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
    void UpdatePreview();
    void UpdateVideo();
    bool RecordVideo(
        std::vector<cv::Mat>& frames
        , const QString& recordDir
        , const VideoFormatType format
        , const double frameRate
        , const int quality
    );


    std::thread threadPreview, threadVideo;
};
