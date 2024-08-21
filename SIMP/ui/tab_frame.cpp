#include "tab_frame.h"
#include "ui_tab_frame.h"
#include "simp_const_value.h"
#include "simp_const_path.h"
#include "dialog_image_processing.h"
#include "worker_frame_processing.h"

#include <QCoreApplication>
#include <QFileDialog>
#include <QMessageBox>

TabFrame::TabFrame(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TabFrame)
    , progressDialog(new QProgressDialog(this))
    , filesystemModel(new QFileSystemModel(this))
    , captureDir(QCoreApplication::applicationDirPath() + SimpConstPath::DIR_CAPTURE_FRAME)
{
    ui->setupUi(this);

    TabFrame::ConnectUI();
    TabFrame::InitUI();
}

TabFrame::~TabFrame()
{
    delete ui;
}

void TabFrame::resizeEvent(QResizeEvent *event)
{
    if (std::abs(this->zoomFactor - 1.0f) <= std::numeric_limits<float>::epsilon())
    {
        ui->gvFrame->fitInView();
    }
}

///////////////////////////////////////////////// ui
void TabFrame::ConnectUI()
{
    connect(ui->gvFrame, &CustomGraphicsView::mousePositionChanged, this, &TabFrame::UpdateMousePosition);

    connect(ui->lvFrame, &QListView::clicked, this, &TabFrame::lvFrame_Click);
    connect(ui->btnOpenDir, &QPushButton::clicked, this, &TabFrame::btnOpenDir_Click);
    connect(ui->btnZoomIn, &QPushButton::clicked, this, &TabFrame::btnZoomIn_Click);
    connect(ui->btnZoomOut, &QPushButton::clicked, this, &TabFrame::btnZoomOut_Click);
    connect(ui->btnFrameProcessing, &QPushButton::clicked, this, &TabFrame::btnFrameProcessing_Click);
    connect(ui->btnFrameSave, &QPushButton::clicked, this, &TabFrame::btnFrameSave_Click);
}

void TabFrame::InitUI()
{
    QDir dirFrame(this->captureDir);
    if (!dirFrame.exists()) {
        dirFrame.mkpath(this->captureDir);
    }

    this->progressDialog->setLabelText("processing image...");
    //this->progressDialog->setCancelButton(nullptr);
    this->progressDialog->setModal(true);
    this->progressDialog->reset();
    this->progressDialog->hide();

    // Set model properties
    this->filesystemModel->setRootPath(this->captureDir);
    this->filesystemModel->setNameFilters(QStringList() << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp" << "*.gif");
    this->filesystemModel->setNameFilterDisables(false);

    ui->lvFrame->setModel(this->filesystemModel);
    ui->lvFrame->setRootIndex(this->filesystemModel->index(this->captureDir)); // Set the root index
    ui->lbDirFrames->setText(this->captureDir);

    // default는 false
    TabFrame::EnableUI(false);
    ui->lvFrame->setEnabled(true);
}

void TabFrame::EnableUI(bool enable)
{
    ui->lvFrame->setEnabled(enable);

    ui->btnZoomIn->setEnabled(enable);
    ui->btnZoomOut->setEnabled(enable);
    ui->btnFrameProcessing->setEnabled(enable);
    ui->btnFrameSave->setEnabled(enable);
}

void TabFrame::UpdateMousePosition(int x, int y, const QColor &color)
{
    QString text = QString("(x: %1, y: %2), (r: %3, g: %4, b: %5)")
                       .arg(x)
                       .arg(y)
                       .arg(color.red())
                       .arg(color.green())
                       .arg(color.blue());
    ui->lbFramePixel->setText(text);  // Assume you have a QLabel named label in your .ui file
}

void TabFrame::lvFrame_Click(const QModelIndex &index)
{
    this->currentFrameIndex = index;
    QString filePath = this->filesystemModel->filePath(this->currentFrameIndex);
    this->currentFrame.load(filePath);

    ui->gvFrame->setImage(this->currentFrame);
    ui->gvFrame->fitInView();

    // listview에서 항목을 선택했으면 true
    TabFrame::EnableUI(true);
}

void TabFrame::btnOpenDir_Click()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty())
    {
        ui->lvFrame->setRootIndex(this->filesystemModel->setRootPath(dir));
        ui->lbDirFrames->setText(dir);
    }
}

void TabFrame::btnZoomIn_Click()
{
    this->zoomFactor += SimpConstValue::ZOOM_VALUE;

    if (this->zoomFactor > SimpConstValue::ZOOM_MAX)
    {
        this->zoomFactor = SimpConstValue::ZOOM_MAX;
    }

    // 현재 창 크기 기준으로 zoom 조절. 실제 이미지 크기로 하면 창 크기를 넘어선다.
    ui->gvFrame->fitInView();
    ui->gvFrame->scale(this->zoomFactor, this->zoomFactor);

    ui->lbZoom->setText(QString("Zoom x%1").arg(this->zoomFactor, 0, 'f', 2));

}

void TabFrame::btnZoomOut_Click()
{
    this->zoomFactor -= SimpConstValue::ZOOM_VALUE;

    if (this->zoomFactor < SimpConstValue::ZOOM_MIN)
    {
        this->zoomFactor = SimpConstValue::ZOOM_MIN;
    }

    // 현재 창 크기 기준으로 zoom 조절. 실제 이미지 크기로 하면 창 크기를 넘어선다.
    ui->gvFrame->fitInView();
    ui->gvFrame->scale(this->zoomFactor, this->zoomFactor);

    ui->lbZoom->setText(QString("Zoom x%1").arg(this->zoomFactor, 0, 'f', 2));
}

void TabFrame::btnFrameProcessing_Click()
{
    DialogImageProcessing dialog(this->lastPresetIndex, this);

    if (dialog.exec() == QDialog::Accepted)
    {
        // 처리가 되는 동안 disable
        TabFrame::EnableUI(false);

        this->lastPresetIndex = dialog.getPresetIndex();

        this->progressDialog->reset();
        this->progressDialog->show();

        QString filePath = this->filesystemModel->filePath(this->currentFrameIndex);

        WorkerFrameProcessing *converter = new WorkerFrameProcessing(
            filePath
            , /*isUpdateBrightnessContrast*/dialog.getBrightnessContrastEnable()
            , /*isUpdateStress*/dialog.getStressEnable()
            , /*isUpdateStretchContrast*/dialog.getStretchContrastEnable()
            , /*brightness*/dialog.getBrightness()
            , /*contrast*/dialog.getContrast()
            , /*stress_radius*/dialog.getStressRadius()
            , /*stress_samples*/dialog.getStressSamples()
            , /*stress_iterations*/dialog.getStressIterations()
            , /*stress_enhance_shadows*/dialog.getStressEnhanceShadows()
            , /*stretch_contrast_keep_colors*/dialog.getStretchContrastKeepColors()
            , /*stretch_contrast_perceptual*/dialog.getStretchContrastNonLinearComponents()
            );

        connect(converter, &WorkerFrameProcessing::cancelled, this, &TabFrame::onFrameConvertingCanceled);
        connect(converter, &WorkerFrameProcessing::finished, this, &TabFrame::onFrameConvertingFinished);
        connect(converter, &WorkerFrameProcessing::finished, converter, &QObject::deleteLater);  // QObject::deleteLater가 thread를 제거함

        // dialog 취소 버튼 클릭하면 converting 중지
        connect(this->progressDialog, &QProgressDialog::canceled, this, [=]() {
            converter->requestInterruption();
        });

        converter->start();
    }
}

void TabFrame::onFrameConvertingCanceled()
{
    this->progressDialog->hide(); // Hide the progress dialog
    QMessageBox::information(this, "Cancel", "Canceled Image Converting.");

    TabFrame::EnableUI(true);
}

void TabFrame::onFrameConvertingFinished(bool success, const QImage& frame)
{
    this->progressDialog->hide(); // Hide the progress dialog

    if (success)
    {
        this->currentFrame = frame;
        ui->gvFrame->setImage(this->currentFrame);

        TabFrame::EnableUI(true);
    }
    else
    {
        QMessageBox::warning(this, "Error", "Could not open the video file.");
    }
}

void TabFrame::btnFrameSave_Click()
{
    QFileInfo fileInfo = this->filesystemModel->fileInfo(this->currentFrameIndex);

    QString dir = fileInfo.absolutePath();
    QString baseName = fileInfo.completeBaseName();
    QString extension = fileInfo.suffix();
    QString filePath = QString("%1/%2_SIMP.%3").arg(dir, baseName, extension);

    // 저장할 때는 현재 gegl이 적용된 이미지를 저장함
    this->currentFrame.save(filePath);
}

