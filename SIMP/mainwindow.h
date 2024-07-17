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
    // void onClickConnectCamera();
    // void handleImageEvent();
    // void handleExpoEvent();
    // void handleTempTintEvent();
    // void handleStillImageEvent();
    // void openCamera();
    // void closeCamera();
    // void startCamera();
    // static void __stdcall eventCallBack(unsigned nEvent, void* pCallbackCtx);

    void onClickOpenVideo();
    void onClickPlayVideo();
    void onClickStopVideo();
    void onVideoStatusChanged(QMediaPlayer::MediaStatus status);
    void SetPlayVideo(bool value);

    void onClickLoadFrames();
    void onSelecteImage(const QModelIndex &index);

    void setupModel(const QString& capturePath);

private:
    Ui::MainWindow *ui;
    QMediaPlayer *mpVideoFile;
    QFileSystemModel *modelFrames;

    // MiicamDeviceV2 miiDevice;
    // HMiicam miiHcam;
    // uchar* pData;
    // unsigned imgWidth;
    // unsigned imgHeight;
    // QTimer* timer;
    // int resolutionIndex;
    // unsigned count;
    //std::unique_ptr<QSortFilterProxyModel> proxyModel;
    //QPixmap currentPixmap;

    // void resizeEvent(QResizeEvent* event) override;

    bool isVideoPlay = false;
};
#endif // MAINWINDOW_H
