#include "tab_video.h"
#include "ui_tab_video.h"
#include "simp_const_path.h"
#include "simp_const_value.h"
#include "simp_const_menu.h"
#include "simp_util.h"
#include "worker_video_loading.h"
#include "worker_video_processing.h"
#include "worker_video_writing_qimage.h"

#include <QFileDialog>
#include <QMessageBox>
#include <thread>

TabVideo::TabVideo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TabVideo)
    , loadingDialog(new QProgressDialog(this))
    , processingDialog(new QProgressDialog(this))
    , videoWritingDialog(new QProgressDialog(this))
    , filesystemModel(new QFileSystemModel(this))
    , dialogImageProcessing(new DialogImageProcessing(this))
    , recordDir(SimpConstPath::DIR_RECORD_VIDEO)
{
    ui->setupUi(this);

    TabVideo::ConnectUI();
    TabVideo::InitUI();

    this->isOn = true;
    this->threadVideo = std::thread(&TabVideo::UpdateVideo, this);
}

TabVideo::~TabVideo()
{
    if (this->dialogImageProcessing)
    {
        delete this->dialogImageProcessing;
    }

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

void TabVideo::onTabActivated()
{
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
    connect(ui->btnVideoSave, &QPushButton::clicked, this, &TabVideo::btnVideoSave_Click);

    connect(ui->btnVideoPlay, &QPushButton::clicked, this, &TabVideo::btnPlayVideo_Click);
    connect(ui->btnVideoStop, &QPushButton::clicked, this, &TabVideo::btnStopVideo_Click);
    connect(ui->btnSaveFrame, &QPushButton::clicked, this, &TabVideo::btnSaveFrame_Click);

    connect(ui->sliderVideoFrame, &QSlider::sliderMoved, this, &TabVideo::sliderVideo_sliderMoved);

    connect(this->dialogImageProcessing, &DialogImageProcessing::applyClicked, this, &TabVideo::ProcessingVideo);
}

void TabVideo::InitUI()
{
    // Check if the captures directory exists, and create it if it doesn't
    QDir dirVideo(this->recordDir);
    if (!dirVideo.exists()) {
        dirVideo.mkpath(this->recordDir);
    }

    this->loadingDialog->setLabelText("Loading video...");
    this->loadingDialog->setRange(0, 100);
    this->loadingDialog->setModal(true);
    this->loadingDialog->reset();
    this->loadingDialog->hide();

    this->processingDialog->setLabelText("processing video...");
    this->processingDialog->setRange(0, 100);
    this->processingDialog->setModal(true);
    this->processingDialog->reset();
    this->processingDialog->hide();

    this->videoWritingDialog->setLabelText("writing video...");
    this->videoWritingDialog->setRange(0, 100);
    this->videoWritingDialog->setModal(true);
    this->videoWritingDialog->reset();
    this->videoWritingDialog->hide();

    // Set model properties
    this->filesystemModel->setRootPath(this->recordDir);
    this->filesystemModel->setNameFilters(QStringList() << "*.avi" << "*.mp4" << "*.wmv");
    this->filesystemModel->setNameFilterDisables(false);

    this->dialogImageProcessing->setWindowTitle("Video Processing");

    ui->lvVideo->setModel(this->filesystemModel);
    ui->lvVideo->setRootIndex(this->filesystemModel->index(this->recordDir)); // Set the root index
    ui->lbDirVideo->setText(this->recordDir);

    // default는 false
    TabVideo::EnableUI(false);
    ui->lvVideo->setEnabled(true);
}

void TabVideo::EnableUI(bool enable)
{
    ui->lvVideo->setEnabled(enable);

    ui->btnZoomIn->setEnabled(enable);
    ui->btnZoomOut->setEnabled(enable);
    ui->btnVideoProcessing->setEnabled(enable);
    ui->btnVideoSave->setEnabled(enable);

    ui->btnVideoPlay->setEnabled(enable);
    ui->btnVideoStop->setEnabled(enable);
    ui->btnSaveFrame->setEnabled(enable);

    ui->sliderVideoFrame->setEnabled(enable);
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
    // 처리가 되는 동안 disable
    TabVideo::EnableUI(false);

    this->currentVideoIndex = index;

    // 일단 update를 중지시킨다.
    this->isVideoPlay = false;

    this->loadingDialog->reset();
    this->loadingDialog->show();

    QString filePath = this->filesystemModel->filePath(this->currentVideoIndex);

    WorkerVideoLoading *loader = new WorkerVideoLoading(filePath);
    connect(loader, &WorkerVideoLoading::progress, this, &TabVideo::onVideoLoadingProgress);
    connect(loader, &WorkerVideoLoading::cancelled, this, &TabVideo::onVideoLoadingCanceled);
    connect(loader, &WorkerVideoLoading::finished, this, &TabVideo::onVideoLoadingFinished);
    connect(loader, &WorkerVideoLoading::finished, loader, &QObject::deleteLater);  // QObject::deleteLater가 thread를 제거함

    // dialog 취소 버튼 클릭하면 loading 중지
    connect(this->loadingDialog, &QProgressDialog::canceled, this, [=]() {
        if (loader) {
            loader->requestInterruption();
        }
    });

    loader->start();
}

void TabVideo::btnOpenDir_Click()
{
    QString dir = QFileDialog::getExistingDirectory(
        this,
        tr("Open Directory"),
        this->filesystemModel->rootPath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

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
    this->dialogImageProcessing->Reset(this->lastPresetIndex);
    this->dialogImageProcessing->show();
}

void TabVideo::btnVideoSave_Click()
{
    // 처리가 되는 동안 disable
    TabVideo::EnableUI(false);

    QFileInfo fileInfo = this->filesystemModel->fileInfo(this->currentVideoIndex);

    QString dir = fileInfo.absolutePath();
    QString baseName = fileInfo.completeBaseName();
    QString extension = fileInfo.suffix();
    QString filePath = QString("%1/%2_SIMP.%3").arg(dir, baseName, extension);    

    this->videoWritingDialog->reset();
    this->videoWritingDialog->show();

    WorkerVideoWritingQImage *writer = new WorkerVideoWritingQImage(
        this->videoFrames
        , /*format*/SimpUtil::getVideoFormat(extension)
        , /*recordSecond*/this->videoFrameRates
        , /*quality*/SimpConstValue::RECORD_QUALITY_DEFAULT
        , /*filePath*/filePath
    );

    connect(writer, &WorkerVideoWritingQImage::progress, this, &TabVideo::onVideoWritingProgress);
    connect(writer, &WorkerVideoWritingQImage::cancelled, this, &TabVideo::onVideoWritingCanceled);
    connect(writer, &WorkerVideoWritingQImage::finished, this, &TabVideo::onVideoWritingFinished);
    connect(writer, &WorkerVideoWritingQImage::finished, writer, &QObject::deleteLater);  // QObject::deleteLater가 thread를 제거함

    // dialog 취소 버튼 클릭하면 converting 중지
    connect(this->videoWritingDialog, &QProgressDialog::canceled, this, [=]() {
        if (writer) {
            writer->requestInterruption();
        }
    });

    writer->start();
}

void TabVideo::btnPlayVideo_Click()
{
    this->isVideoPlay = !this->isVideoPlay;
    ui->btnVideoPlay->setText(!this->isVideoPlay ? SimpConstMenu::BTN_PLAY : SimpConstMenu::BTN_PAUSE);
}

void TabVideo::btnStopVideo_Click()
{
    this->isVideoPlay = false;
    this->currentFrameIndex = 0;

    // play를 중지시켰으므로 직접 업데이트 한다.
    this->currentFrame = this->videoFrames[this->currentFrameIndex];
    TabVideo::UpdateVideoUI();
}

void TabVideo::btnSaveFrame_Click()
{
    QDir dir(SimpConstPath::DIR_CAPTURE_FRAME);
    if (!dir.exists())
    {
        dir.mkpath(SimpConstPath::DIR_CAPTURE_FRAME);
    }

    QFileInfo fileInfo = this->filesystemModel->fileInfo(this->currentVideoIndex);
    QString fileName = QString("%1_%2").arg(fileInfo.completeBaseName()).arg(QString::number(this->currentFrameIndex + 1));
    QString filePath = dir.absoluteFilePath(fileName + SimpConstPath::EXTENSION_CAPTURE_IMAGE);

    this->currentFrame.save(filePath);
}

void TabVideo::sliderVideo_sliderMoved(int position)
{
    this->currentFrameIndex = position;

    // play 중이 아니면 직접 업데이트한다.
    if (!this->isVideoPlay)
    {
        this->currentFrame = this->videoFrames[this->currentFrameIndex];
        TabVideo::UpdateVideoUI();
    }
}

void TabVideo::onVideoLoadingProgress(int current, int total)
{
    int value = ((current + 1) * 100) / total;
    this->loadingDialog->setValue(value);
}

void TabVideo::onVideoLoadingCanceled()
{
    this->loadingDialog->hide(); // Hide the progress dialog
    QMessageBox::information(this, "Cancel", "Canceled Video Loading.");

    TabVideo::EnableUI(true);
}

void TabVideo::onVideoLoadingFinished(bool success, const std::vector<QImage>& frames, double frameRate, int totalFrames)
{
    this->loadingDialog->hide(); // Hide the progress dialog

    if (success)
    {
        this->videoFrames = frames;
        this->videoFrameRates = frameRate;
        this->videoTotalFrame = totalFrames;
        this->currentFrameIndex = 0;
        this->videoDelay = SimpConstValue::SECOND / this->videoFrameRates;

        ui->sliderVideoFrame->setRange(0, this->videoTotalFrame - 1);
        ui->sliderVideoFrame->setSingleStep(1);
        ui->sliderVideoFrame->setPageStep(10);
        ui->sliderVideoFrame->setValue(0);

        ui->lbVideoFrame->setText(QString("%1 / %2").arg(ui->sliderVideoFrame->value()).arg(this->videoTotalFrame));
        ui->btnVideoPlay->setText("Play");

        // 첫 프레임을 UI에 띄운다.
        this->currentFrame = this->videoFrames[this->currentFrameIndex];
        TabVideo::UpdateVideoUI();

        // 처리가 완료 되었으므로 enable
        TabVideo::EnableUI(true);
    }
    else
    {
        QMessageBox::warning(this, "Error", "Could not open the video file.");
    }
}

void TabVideo::onVideoConvertingProgress(int current, int total)
{
    int value = ((current + 1) * 100) / total;
    this->processingDialog->setValue(value);
}

void TabVideo::onVideoConvertingCanceled()
{
    this->processingDialog->hide(); // Hide the progress dialog
    QMessageBox::information(this, "Cancel", "Canceled Video Converting.");

    TabVideo::EnableUI(true);
}

void TabVideo::onVideoConvertingFinished(bool success, const std::vector<QImage>& frames)
{
    this->processingDialog->hide(); // Hide the progress dialog

    if (success)
    {
        this->videoFrames = frames;
        this->currentFrameIndex = 0;

        ui->sliderVideoFrame->setValue(0);
        ui->btnVideoPlay->setText("Play");

        // 첫 프레임을 UI에 띄운다.
        this->currentFrame = this->videoFrames[this->currentFrameIndex];
        TabVideo::UpdateVideoUI();

        // 처리가 완료 되었으므로 enable
        TabVideo::EnableUI(true);
    }
    else
    {
        QMessageBox::warning(this, "Error", "Could not open the video file.");
    }
}

void TabVideo::onVideoWritingProgress(int current, int total)
{
    int value = ((current + 1) * 100) / total;
    this->videoWritingDialog->setValue(value);
}

void TabVideo::onVideoWritingCanceled()
{
    this->videoWritingDialog->hide(); // Hide the progress dialog
    QMessageBox::information(this, "Cancel", "Canceled Video Writing.");

    TabVideo::EnableUI(true);
}

void TabVideo::onVideoWritingFinished(bool success)
{
    this->videoWritingDialog->hide(); // Hide the progress dialog

    if (success)
    {
        // 처리가 완료 되었으므로 enable
        TabVideo::EnableUI(true);
    }
    else
    {
        QMessageBox::warning(this, "Error", "Could not write the video file.");
    }
}

void TabVideo::UpdateVideoUI()
{
    ui->gvVideo->setImage(this->currentFrame);

    if (std::abs(zoomFactor - 1.0f) <= std::numeric_limits<float>::epsilon())
    {
        ui->gvVideo->fitInView();
    }

    ui->lbVideoFrame->setText(QString("%1 / %2").arg(this->currentFrameIndex + 1).arg(this->videoTotalFrame));
    ui->sliderVideoFrame->setValue(this->currentFrameIndex);

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
            if (this->currentFrameIndex < this->videoTotalFrame)
            {
                this->currentFrame = this->videoFrames[this->currentFrameIndex++];
            }
            else
            {
                this->currentFrameIndex = 0;
                this->currentFrame = this->videoFrames[this->currentFrameIndex];
                this->isVideoPlay = false;
            }

            QMetaObject::invokeMethod(this, "UpdateVideoUI", Qt::QueuedConnection);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(this->videoDelay)); // Frame rate delay
    }
}

void TabVideo::ProcessingVideo()
{
    // 처리가 되는 동안 disable
    TabVideo::EnableUI(false);

    this->lastPresetIndex = this->dialogImageProcessing->GetPresetIndex();

    // 일단 update를 중지시킨다.
    this->isVideoPlay = false;

    this->processingDialog->reset();
    this->processingDialog->show();

    QString filePath = this->filesystemModel->filePath(this->currentVideoIndex);

    WorkerVideoProcessing *converter = new WorkerVideoProcessing(
        filePath
        , /*isUpdateBrightnessContrast*/this->dialogImageProcessing->GetBrightnessContrastEnable()
        , /*isUpdateStress*/this->dialogImageProcessing->GetStressEnable()
        , /*isUpdateStretchContrast*/this->dialogImageProcessing->GetStretchContrastEnable()
        , /*brightness*/this->dialogImageProcessing->GetBrightness()
        , /*contrast*/this->dialogImageProcessing->GetContrast()
        , /*stress_radius*/this->dialogImageProcessing->GetStressRadius()
        , /*stress_samples*/this->dialogImageProcessing->GetStressSamples()
        , /*stress_iterations*/this->dialogImageProcessing->GetStressIterations()
        , /*stress_enhance_shadows*/this->dialogImageProcessing->GetStressEnhanceShadows()
        , /*stretch_contrast_keep_colors*/this->dialogImageProcessing->GetStretchContrastKeepColors()
        , /*stretch_contrast_perceptual*/this->dialogImageProcessing->GetStretchContrastNonLinearComponents()
        );

    connect(converter, &WorkerVideoProcessing::progress, this, &TabVideo::onVideoConvertingProgress);
    connect(converter, &WorkerVideoProcessing::cancelled, this, &TabVideo::onVideoConvertingCanceled);
    connect(converter, &WorkerVideoProcessing::finished, this, &TabVideo::onVideoConvertingFinished);
    connect(converter, &WorkerVideoProcessing::finished, converter, &QObject::deleteLater);  // QObject::deleteLater가 thread를 제거함

    // dialog 취소 버튼 클릭하면 converting 중지
    connect(this->processingDialog, &QProgressDialog::canceled, this, [=]() {
        if (converter) {
            converter->requestInterruption();
        }
    });

    converter->start();
}
