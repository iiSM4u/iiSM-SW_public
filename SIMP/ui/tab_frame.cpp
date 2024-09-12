#include "tab_frame.h"
#include "ui_tab_frame.h"
#include "simp_gegl.h"
#include "simp_const_key.h"
#include "simp_const_value.h"
#include "simp_const_path.h"
#include "simp_util.h"
#include "dialog_image_processing.h"
#include "dialog_contrast_curve.h"
#include "worker_frame_processing.h"

#include <QFileDialog>
#include <QMessageBox>

TabFrame::TabFrame(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TabFrame)
    , progressDialog(new QProgressDialog(this))
    , filesystemModel(new QFileSystemModel(this))
    , dialogImageProcessing(new DialogImageProcessing(this))
    , captureDir(SimpConstPath::DIR_CAPTURE_FRAME)
{
    ui->setupUi(this);

    TabFrame::LoadPresets();
    TabFrame::ConnectUI();
    TabFrame::InitUI();
}

TabFrame::~TabFrame()
{
    if (this->dialogImageProcessing)
    {
        delete this->dialogImageProcessing;
    }

    delete ui;
}

void TabFrame::resizeEvent(QResizeEvent *event)
{
    if (std::abs(this->zoomFactor - 1.0f) <= std::numeric_limits<float>::epsilon())
    {
        ui->gvFrame->fitInView();
    }
}

void TabFrame::onTabActivated()
{
    int index = ui->cbCurvePreset->currentIndex();

    TabFrame::LoadPresets();
    TabFrame::UpdatePresetContrastCurve(this->presetsContrastCurve, index);
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

    connect(ui->btnCurveSetting, &QPushButton::clicked, this, &TabFrame::btnCurveSetting_Click);
    connect(ui->cbCurvePreset, &QComboBox::currentIndexChanged, this, &TabFrame::cbCurvePreset_SelectedIndexChanged);

    connect(this->dialogImageProcessing, &DialogImageProcessing::applyClicked, this, &TabFrame::ProcessingFrame);
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

    this->dialogImageProcessing->setWindowTitle("Image Processing");

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

    ui->btnCurveSetting->setEnabled(enable);
    ui->cbCurvePreset->setEnabled(enable);
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

    TabFrame::UpdateFrame(ui->cbCurvePreset->currentIndex());

    // listview에서 항목을 선택했으면 true
    TabFrame::EnableUI(true);
}

void TabFrame::btnOpenDir_Click()
{
    QString dir = QFileDialog::getExistingDirectory(
        this,
        tr("Open Directory"),
        this->filesystemModel->rootPath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

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
    this->dialogImageProcessing->Reset(this->lastPresetIndex);
    this->dialogImageProcessing->show();
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

void TabFrame::btnCurveSetting_Click()
{
    int presetIndex = ui->cbCurvePreset->currentIndex();
    DialogContrastCurve dialog(this->presetsContrastCurve, presetIndex  - 1, presetIndex > 0, this);
    connect(&dialog, &DialogContrastCurve::contrastCurveUpdated, this, &TabFrame::UpdateContrastCurvePoints);

    if (dialog.exec() == QDialog::Accepted)
    {
        this->presetsContrastCurve = dialog.getPresets();
        bool enable = dialog.getEnable();

        QJsonArray jsonArray;
        SimpUtil::convertPresetsImageCurveToJsonArray(this->presetsContrastCurve, jsonArray);

        QString pathPreset = SimpConstPath::PATH_JSON_CONTRAST_CURVE;
        SimpUtil::saveJsonFile(pathPreset, jsonArray);

        // 여기에는 none이 있기 때문에 index에 +1 해야 함.
        int index = enable ? dialog.getSelectedIndex() + 1 : 0;
        TabFrame::UpdatePresetContrastCurve(this->presetsContrastCurve, index);
    }
    else
    {
        int index = ui->cbCurvePreset->currentIndex();

        // 선택된 preset이 있었으면 그것으로 업데이트
        if (index > 0)
        {
            TabFrame::UpdateContrastCurvePoints(this->presetsContrastCurve[index - 1].GetPoints());
        }
    }
}

void TabFrame::cbCurvePreset_SelectedIndexChanged(int index)
{
    TabFrame::UpdateFrame(index);
}

void TabFrame::UpdatePresetContrastCurve(const std::vector<PresetContrastCurve>& presets, const int index)
{
    ui->cbCurvePreset->clear();

    ui->cbCurvePreset->addItem(SimpConstKey::NONE);

    if (presets.size() > 0)
    {
        for (const PresetContrastCurve& preset : presets)
        {
            QString message = QString("index: %1, points: %2")
                                  .arg(preset.GetIndex()) // 'f' for floating point, 2 decimal places
                                  .arg(preset.GetPoints().size());
            ui->cbCurvePreset->addItem(message);
        }
    }

    ui->cbCurvePreset->setCurrentIndex(index);
}

void TabFrame::UpdateFrame(int presetIndex)
{
    // 0이면 none
    if (presetIndex > 0)
    {
        PresetContrastCurve preset = this->presetsContrastCurve[presetIndex - 1];
        TabFrame::UpdateContrastCurvePoints(preset.GetPoints());
    }
    // 원본 복귀
    else
    {
        ui->gvFrame->setImage(this->currentFrame);
        ui->gvFrame->fitInView();
    }
}

void TabFrame::UpdateContrastCurvePoints(const QVector<QPointF>& points)
{
    // gegl에서는 rgba를 받기 때문에 무조건 rgba로 바꿔야 한다.
    QImage formattedSource = this->currentFrame.convertToFormat(QImage::Format_RGBA8888);

    SimpGEGL::UpdateContrastCurve(formattedSource, points);

    // 원본은 그대로 둔 상태에서 변환한다
    ui->gvFrame->setImage(formattedSource.convertToFormat(QImage::Format_RGB888));
}

void TabFrame::LoadPresets()
{
    QJsonArray jsonArray;

    if (SimpUtil::loadJsonFile(SimpConstPath::PATH_JSON_CONTRAST_CURVE, jsonArray))
    {
        this->presetsContrastCurve = SimpUtil::convertJsonToPresetsImageCurve(jsonArray);
    }
}

void TabFrame::ProcessingFrame()
{
    // 처리가 되는 동안 disable
    TabFrame::EnableUI(false);

    this->lastPresetIndex = this->dialogImageProcessing->GetPresetIndex();

    this->progressDialog->reset();
    this->progressDialog->show();

    QString filePath = this->filesystemModel->filePath(this->currentFrameIndex);

    WorkerFrameProcessing *converter = new WorkerFrameProcessing(
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

    connect(converter, &WorkerFrameProcessing::cancelled, this, &TabFrame::onFrameConvertingCanceled);
    connect(converter, &WorkerFrameProcessing::finished, this, &TabFrame::onFrameConvertingFinished);
    connect(converter, &WorkerFrameProcessing::finished, converter, &QObject::deleteLater);  // QObject::deleteLater가 thread를 제거함

    // dialog 취소 버튼 클릭하면 converting 중지
    connect(this->progressDialog, &QProgressDialog::canceled, this, [=]() {
        if (converter) {
            converter->requestInterruption();
        }
    });

    converter->start();
}
