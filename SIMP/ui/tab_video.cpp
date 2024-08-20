#include "tab_video.h"
#include "ui_tab_video.h"
#include "dialog_image_processing.h"
#include "simp_const_path.h"
#include "simp_const_value.h"
#include "simp_const_menu.h"
#include "video_loader.h"

#include <QCoreApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <thread>

TabVideo::TabVideo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TabVideo)
    , progressDialog(new QProgressDialog(this))
    , filesystemModel(new QFileSystemModel(this))
    , recordDir(QCoreApplication::applicationDirPath() + SimpConstPath::DIR_RECORD_VIDEO)
{
    ui->setupUi(this);

    TabVideo::ConnectUI();
    TabVideo::InitUI();

    this->isOn = true;
    this->threadVideo = std::thread(&TabVideo::UpdateVideo, this);
}

TabVideo::~TabVideo()
{
    delete ui;
}

void TabVideo::closeEvent(QCloseEvent*)
{
    this->isOn = false;

    if (this->threadVideo.joinable()) {
        this->threadVideo.join();
    }
}

void TabVideo::resizeEvent(QResizeEvent *event)
{
    // play 중일 떄는 play하면서 업데이트하므로 하지 않는다.
    if (!this->isVideoPlay && std::abs(this->zoomFactor - 1.0f) <= std::numeric_limits<float>::epsilon())
    {
        ui->gvVideo->fitInView();
    }
}


///////////////////////////////////////////////// ui
void TabVideo::ConnectUI()
{
    connect(ui->gvVideo, &CustomGraphicsView::mousePositionChanged, this, &TabVideo::UpdateMousePosition);

    connect(ui->lvVideo, &QListView::clicked, this, &TabVideo::lvVideo_Click);
    connect(ui->btnOpenDir, &QPushButton::clicked, this, &TabVideo::btnOpenDir_Click);
    connect(ui->btnZoomIn, &QPushButton::clicked, this, &TabVideo::btnZoomIn_Click);
    connect(ui->btnZoomOut, &QPushButton::clicked, this, &TabVideo::btnZoomOut_Click);
    connect(ui->btnVideoProcessing, &QPushButton::clicked, this, &TabVideo::btnVideoProcessing_Click);

    connect(ui->btnVideoPlay, &QPushButton::clicked, this, &TabVideo::btnPlayVideo_Click);
    connect(ui->btnVideoStop, &QPushButton::clicked, this, &TabVideo::btnStopVideo_Click);

    connect(ui->sliderVideoFrame, &QSlider::sliderMoved, this, &TabVideo::sliderVideo_sliderMoved);
}

void TabVideo::InitUI()
{
    // Check if the captures directory exists, and create it if it doesn't
    QDir dirVideo(this->recordDir);
    if (!dirVideo.exists()) {
        dirVideo.mkpath(this->recordDir);
    }

    // Set model properties
    this->filesystemModel->setRootPath(this->recordDir);
    this->filesystemModel->setNameFilters(QStringList() << "*.avi" << "*.mp4" << "*.wmv");
    this->filesystemModel->setNameFilterDisables(false);

    ui->lvVideo->setModel(this->filesystemModel);
    ui->lvVideo->setRootIndex(this->filesystemModel->index(this->recordDir)); // Set the root index
    ui->lbDirVideo->setText(this->recordDir);

    ui->btnVideoPlay->setEnabled(false);
    ui->btnVideoStop->setEnabled(false);
    ui->sliderVideoFrame->setEnabled(false);

    this->progressDialog->setLabelText("Loading video...");
    this->progressDialog->setCancelButton(nullptr);
    this->progressDialog->setRange(0, 100);
    this->progressDialog->setModal(true);
    this->progressDialog->reset();
}

void TabVideo::UpdateMousePosition(int x, int y, const QColor &color)
{
    QString text = QString("(x: %1, y: %2), (r: %3, g: %4, b: %5)")
                       .arg(x)
                       .arg(y)
                       .arg(color.red())
                       .arg(color.green())
                       .arg(color.blue());
    ui->lbVideoPixel->setText(text);  // Assume you have a QLabel named label in your .ui file
}

void TabVideo::lvVideo_Click(const QModelIndex &index)
{
    // 일단 update를 중지시킨다.
    this->isVideoPlay = false;

    QString filePath = this->filesystemModel->filePath(index);

    this->progressDialog->reset();
    this->progressDialog->show();

    VideoLoader *loader = new VideoLoader(filePath);
    connect(loader, &VideoLoader::progress, this, &TabVideo::onVideoLoadingProgress);
    connect(loader, &VideoLoader::finished, this, &TabVideo::onVideoLoadingFinished);
    connect(loader, &VideoLoader::finished, loader, &QObject::deleteLater);

    loader->start();
}

void TabVideo::btnOpenDir_Click()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty())
    {
        ui->lvVideo->setRootIndex(this->filesystemModel->setRootPath(dir));
        ui->lbDirVideo->setText(dir);
    }
}

void TabVideo::btnZoomIn_Click()
{
    this->zoomFactor += SimpConstValue::ZOOM_VALUE;

    if (this->zoomFactor > SimpConstValue::ZOOM_MAX)
    {
        this->zoomFactor = SimpConstValue::ZOOM_MAX;
    }

    // 현재 창 크기 기준으로 zoom 조절. 실제 이미지 크기로 하면 창 크기를 넘어선다.
    ui->gvVideo->fitInView();
    ui->gvVideo->scale(this->zoomFactor, this->zoomFactor);

    ui->lbZoom->setText(QString("Zoom x%1").arg(this->zoomFactor, 0, 'f', 2));

}

void TabVideo::btnZoomOut_Click()
{
    this->zoomFactor -= SimpConstValue::ZOOM_VALUE;

    if (this->zoomFactor < SimpConstValue::ZOOM_MIN)
    {
        this->zoomFactor = SimpConstValue::ZOOM_MIN;
    }

    // 현재 창 크기 기준으로 zoom 조절. 실제 이미지 크기로 하면 창 크기를 넘어선다.
    ui->gvVideo->fitInView();
    ui->gvVideo->scale(this->zoomFactor, this->zoomFactor);

    ui->lbZoom->setText(QString("Zoom x%1").arg(this->zoomFactor, 0, 'f', 2));
}

void TabVideo::btnVideoProcessing_Click()
{
    DialogImageProcessing dialog(this);

    if (dialog.exec() == QDialog::Accepted)
    {
    }
}

void TabVideo::onVideoLoadingProgress(int value)
{
    this->progressDialog->setValue(value);
}

void TabVideo::onVideoLoadingFinished(bool success, const std::vector<QImage>& frames, double frameRate, int totalFrames)
{
    this->progressDialog->hide(); // Hide the progress dialog

    if (success)
    {
        this->videoFrames = frames;
        this->videoFrameRates = frameRate;
        this->videoTotalFrame = totalFrames;
        this->currentFrame = 0;

        ui->sliderVideoFrame->setRange(0, this->videoTotalFrame - 1);
        ui->sliderVideoFrame->setSingleStep(1);
        ui->sliderVideoFrame->setPageStep(10);
        ui->sliderVideoFrame->setValue(0);

        ui->lbVideoFrame->setText(QString("%1 / %2").arg(ui->sliderVideoFrame->value()).arg(this->videoTotalFrame));

        ui->btnVideoPlay->setEnabled(true);
        ui->btnVideoStop->setEnabled(true);
        ui->sliderVideoFrame->setEnabled(true);

        ui->btnVideoPlay->setText("Play");
        this->isVideoPlay = false;  // 자동실행 안 함

        // 첫 프레임을 띄운다.
        this->resultVideo = this->videoFrames[this->currentFrame];
        TabVideo::UpdateVideoUI();
    }
    else
    {
        QMessageBox::warning(this, "Error", "Could not open the video file.");
    }
}

void TabVideo::btnPlayVideo_Click()
{
    this->isVideoPlay = !this->isVideoPlay;
    ui->btnVideoPlay->setText(!this->isVideoPlay ? SimpConstMenu::BTN_PLAY : SimpConstMenu::BTN_PAUSE);
}

void TabVideo::btnStopVideo_Click()
{
    this->isVideoPlay = false;
    this->currentFrame = 0;

    // play를 중지시켰으므로 직접 업데이트 한다.
    this->resultVideo = this->videoFrames[this->currentFrame];
    TabVideo::UpdateVideoUI();
}

void TabVideo::sliderVideo_sliderMoved(int position)
{
    this->currentFrame = position;

    // play 중이 아니면 직접 업데이트한다.
    if (!this->isVideoPlay)
    {
        this->resultVideo = this->videoFrames[this->currentFrame];
        TabVideo::UpdateVideoUI();
    }
}

void TabVideo::UpdateVideoUI()
{
    ui->gvVideo->setImage(this->resultVideo);

    if (std::abs(zoomFactor - 1.0f) <= std::numeric_limits<float>::epsilon())
    {
        ui->gvVideo->fitInView();
    }

    ui->lbVideoFrame->setText(QString("%1 / %2").arg(this->currentFrame + 1).arg(this->videoTotalFrame));
    ui->sliderVideoFrame->setValue(this->currentFrame);

    if (!this->isVideoPlay)
    {
        ui->btnVideoPlay->setText(SimpConstMenu::BTN_PLAY);
    }
}

void TabVideo::UpdateVideo()
{
    while (this->isOn)
    {
        if (this->isVideoPlay)
        {
            if (this->currentFrame < this->videoTotalFrame)
            {
                this->resultVideo = this->videoFrames[this->currentFrame++];
            }
            else
            {
                this->currentFrame = 0;
                this->resultVideo = this->videoFrames[this->currentFrame];
                this->isVideoPlay = false;
            }

            QMetaObject::invokeMethod(this, "UpdateVideoUI", Qt::QueuedConnection);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(SimpConstValue::DELAY_VIDEO)); // Frame rate delay
    }
}
