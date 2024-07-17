#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include "PixelFormatType.h"
#include "constants.h"

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

    // frame
    void btnLoadFrame_Click();


    // comm
    void onTimerCallback();


    // mii camera
    void FindCamera();
    void openCamera();
    void closeCamera();
    void startCamera();

    void onMiiCameraCallback(unsigned nEvent);
    void handleImageEvent();
    void handleExpoEvent();
    void handleTempTintEvent();
    void handleStillImageEvent();
    static void __stdcall eventCallBack(unsigned nEvent, void* pCallbackCtx);


    void onVideoStatusChanged(QMediaPlayer::MediaStatus status);
    void SetPlayVideo(bool value);

    void onSelecteImage(const QModelIndex &index);

    void setupModel(const QString& capturePath);

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
#endif // MAINWINDOW_H
