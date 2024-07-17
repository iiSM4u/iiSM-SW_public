#pragma once

#include <QMainWindow>
#include <QMediaPlayer>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QTimer>
#include <QLabel>
#include <QListView>
#include <QRegularExpression>
#include <QMessageBox>
#include <QPainter>

#include <miicam.h>

// C 라이브러리를 참조할 떄는 extern C로 묶는 것이 링킹 문제를 방지하는데 도움이 됨
extern "C" {
#include <gegl.h>
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
    void btnBrightnessContrast_Click();


    // custom
    void SetupModel(const QString& capturePath);
    void SetPlayVideo(bool value);

private:
    Ui::MainWindow *ui;

    QMediaPlayer *mpVideoFile;
    QFileSystemModel *modelFrames;
    QTimer* timer;
    //QPixmap currentPixmap;

    MiicamDeviceV2 miiDevice;
    HMiicam miiHcam = nullptr;

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
