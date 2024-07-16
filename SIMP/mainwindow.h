#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <memory>
#include <QLabel>
#include <QListView>
#include <QRegularExpression>

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

private slots:
    void onClickLoadFrames();
    void onSelecteImage(const QModelIndex &index);

    void onClickOpenVideo();
    void onClickPlayVideo();
    void onClickStopVideo();
    void onVideoStatusChanged(QMediaPlayer::MediaStatus status);
    void SetPlayVideo(bool value);

private:
    Ui::MainWindow *ui;
    QMediaPlayer *mediaPlayer;
    std::unique_ptr<QFileSystemModel> model;
    //std::unique_ptr<QSortFilterProxyModel> proxyModel;
    //QPixmap currentPixmap;

    // void resizeEvent(QResizeEvent* event) override;

    bool isVideoPlay = false;
};
#endif // MAINWINDOW_H
