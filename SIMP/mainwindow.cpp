#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , model(std::make_unique<QFileSystemModel>(this))
    , mediaPlayer(new QMediaPlayer(this))
{
    ui->setupUi(this);    

    mediaPlayer->setVideoOutput(ui->videoFile);

    // Set up the file system model to display only images
    model->setRootPath("");
    model->setNameFilters(QStringList() << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp" << "*.gif");
    model->setNameFilterDisables(false);

    // Set up the QListView
    ui->lvFrames->setModel(model.get());
    connect(ui->lvFrames, &QListView::clicked, this, &MainWindow::onSelecteImage);
    connect(ui->btnLoadFrame, &QPushButton::clicked, this, &MainWindow::onClickLoadFrames);

    connect(ui->btnLoadVideo, &QPushButton::clicked, this, &MainWindow::onClickOpenVideo);
    connect(ui->btnPlayVideo, &QPushButton::clicked, this, &MainWindow::onClickPlayVideo);
    connect(ui->btnStopVideo, &QPushButton::clicked, this, &MainWindow::onClickStopVideo);

    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onVideoStatusChanged);

    ui->btnPlayVideo->setEnabled(false);
    ui->btnStopVideo->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onClickLoadFrames()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty())
    {
        ui->lvFrames->setRootIndex(model->setRootPath(dir));
        ui->lbDirFrames->setText(dir);
    }
}


void MainWindow::onSelecteImage(const QModelIndex &index)
{
    QString filePath = model->filePath(index);
    QPixmap pixmap(filePath);
    ui->lbFrameCapture->setPixmap(pixmap.scaled(ui->lbFrameCapture->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}


void MainWindow::onClickOpenVideo()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Video"), "", tr("Video Files (*.mp4 *.avi *.mkv)"));

    if (!fileName.isEmpty())
    {
        mediaPlayer->setSource(QUrl::fromLocalFile(fileName));
        ui->lbPathVideo->setText(fileName);

        ui->btnPlayVideo->setEnabled(true);
        ui->btnStopVideo->setEnabled(true);

        // 일단 자동으로 시작하게 한다.
        mediaPlayer->play();
        MainWindow::SetPlayVideo(true);
    }
}


void MainWindow::onClickPlayVideo()
{
    if (isVideoPlay)
    {
        mediaPlayer->pause();
        MainWindow::SetPlayVideo(false);
    }
    else
    {
        mediaPlayer->play();
        MainWindow::SetPlayVideo(true);
    }
}


void MainWindow::onClickStopVideo()
{
    mediaPlayer->stop();
    MainWindow::SetPlayVideo(false);
}

void MainWindow::onVideoStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia)
    {
        MainWindow::SetPlayVideo(false);
    }
}

void MainWindow::SetPlayVideo(bool value)
{
    if (!value)
    {
        ui->btnPlayVideo->setText("Play");
        isVideoPlay = false;
    }
    else
    {
        ui->btnPlayVideo->setText("Pause");
        isVideoPlay = true;
    }
}

// void MainWindow::resizeEvent(QResizeEvent* event)
// {
//     QMainWindow::resizeEvent(event);
//     if (!currentPixmap.isNull()) {
//         ui->lbFrameCapture->setPixmap(currentPixmap.scaled(ui->lbFrameCapture->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
//     }
// }
