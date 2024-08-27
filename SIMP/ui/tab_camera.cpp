#include "tab_camera.h"
#include "ui_tab_camera.h"
#include "simp_gegl.h"
#include "simp_util.h"
#include "simp_logger.h"
#include "simp_const_value.h"
#include "simp_const_key.h"
#include "simp_const_path.h"
#include "simp_const_menu.h"
#include "simp_const_format.h"
#include "pixel_format_type.h"
#include "dialog_record_option.h"
#include "dialog_contrast_curve.h"
#include "worker_video_writing_cv.h"

#include <QTimer>
#include <QMessageBox>
#include <QDir>
#include <thread>

TabCamera::TabCamera(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TabCamera)
    , timerFPS(new QTimer(this))
    , timerVideoRecord(new QTimer(this))
    , videoWritingDialog(new QProgressDialog(this))
    , btnGroupCooling(new QButtonGroup(this))
    , recordDir(SimpConstPath::DIR_RECORD_VIDEO)
    , captureDir(SimpConstPath::DIR_CAPTURE_FRAME)
{
    ui->setupUi(this);

    TabCamera::LoadPresets();
    TabCamera::ConnectUI();
    TabCamera::InitUI();
    TabCamera::UpdatePresetContrastCurve(this->presetsContrastCurve);

    // 일단 false로 시작
    TabCamera::EnableUI(false);

    ui->sliderTemperature->setEnabled(false);
    ui->editTemperature->setEnabled(false);

    // thread 시작
    this->isCameraOn = true;
    this->threadCamera = std::thread(&TabCamera::UpdatePreview, this);

    // ui 초기화 후에 우선 카메라부터 찾는다.
    TabCamera::FindCamera();
}

TabCamera::~TabCamera()
{
    delete ui;
}

void TabCamera::closeEvent(QCloseEvent*)
{
    this->isCameraOn = false;

    if (this->threadCamera.joinable())
    {
        this->threadCamera.join();
    }

    TabCamera::CloseCamera();
}

///////////////////////////////////////////////// ui
void TabCamera::ConnectUI()
{
    connect(this, &TabCamera::evtCallback, this, &TabCamera::onMiiCameraCallback);
    connect(this->timerFPS, &QTimer::timeout, this, &TabCamera::onTimerFpsCallback);

    connect(ui->gvCamera, &CustomGraphicsView::mousePositionChanged, this, &TabCamera::UpdateMousePosition);

    connect(ui->cbResolution, &QComboBox::currentIndexChanged, this, &TabCamera::cbResoution_SelectedIndexChanged);
    connect(ui->cbFormat, &QComboBox::currentIndexChanged, this, &TabCamera::cbFormat_SelectedIndexChanged);

    connect(ui->btnPlayCamera, &QPushButton::clicked, this, &TabCamera::btnPlayCamera_Click);
    connect(ui->btnStopCamera, &QPushButton::clicked, this, &TabCamera::btnStopCamera_Click);
    connect(ui->btnCaptureCamera, &QPushButton::clicked, this, &TabCamera::btnCaptureCamera_Click);
    connect(ui->btnRecordOn, &QPushButton::clicked, this, &TabCamera::btnRecordOn_Click);
    connect(ui->btnRecordOption, &QPushButton::clicked, this, &TabCamera::btnRecordOption_Click);

    connect(ui->sliderExposureTime, &QSlider::sliderMoved, this, &TabCamera::sliderExposureTime_sliderMoved);
    connect(ui->editExposureTime, &CustomPlainTextEdit::editingFinished, this, &TabCamera::editExposureTime_editingFinished);

    connect(ui->sliderGain, &QSlider::sliderMoved, this, &TabCamera::sliderGain_sliderMoved);
    connect(ui->editGain, &CustomPlainTextEdit::editingFinished, this, &TabCamera::editGain_editingFinished);

    connect(ui->sliderContrast, &QSlider::sliderMoved, this, &TabCamera::sliderContrast_sliderMoved);
    connect(ui->editContrast, &CustomPlainTextEdit::editingFinished, this, &TabCamera::editContrast_editingFinished);

    connect(ui->sliderGamma, &QSlider::sliderMoved, this, &TabCamera::sliderGamma_sliderMoved);
    connect(ui->editGamma, &CustomPlainTextEdit::editingFinished, this, &TabCamera::editGamma_editingFinished);

    connect(ui->btnCurveSetting, &QPushButton::clicked, this, &TabCamera::btnCurveSetting_Click);
    connect(ui->cbCurvePreset, &QComboBox::currentIndexChanged, this, &TabCamera::cbCurvePreset_SelectedIndexChanged);

    connect(ui->chkDarkFieldCorrection, &QCheckBox::checkStateChanged, this, &TabCamera::chkDarkFieldCorrection_CheckedChanged);
    connect(ui->btnDarkFieldCorrection, &QPushButton::clicked, this, &TabCamera::btnDarkFieldCorrection_Click);

    this->btnGroupCooling->addButton(ui->rbCoolingOn, 1);
    this->btnGroupCooling->addButton(ui->rbCoolingOff, 2);
    connect(this->btnGroupCooling, &QButtonGroup::idClicked, this, &TabCamera::btnGroupCooling_Click);

    connect(ui->sliderTemperature, &QSlider::sliderMoved, this, &TabCamera::sliderTemperature_sliderMoved);
    connect(ui->editTemperature, &CustomPlainTextEdit::editingFinished, this, &TabCamera::editTemperature_editingFinished);

    connect(ui->btnZoomIn, &QPushButton::clicked, this, &TabCamera::btnZoomIn_Click);
    connect(ui->btnZoomOut, &QPushButton::clicked, this, &TabCamera::btnZoomOut_Click);
}

void TabCamera::InitUI()
{
    ui->cbFormat->clear();
    for (const auto& format : {PixelFormatType::RGB24, PixelFormatType::Raw})
    //for (const auto& format : {PixelFormatType::RGB24, PixelFormatType::RGB32, PixelFormatType::Raw})
    {
        ui->cbFormat->addItem(toString(format));
    }
    ui->cbFormat->setCurrentIndex(0);

    // init gain, contrast, gamma
    ui->sliderGain->setMinimum(MIICAM_EXPOGAIN_MIN);
    ui->sliderGain->setMaximum(SimpConstValue::MIICAM_EXPOGAIN_MAX);
    ui->sliderGain->setValue(MIICAM_EXPOGAIN_DEF);
    ui->editGain->setPlainText(QString::number(MIICAM_EXPOGAIN_DEF));

    ui->sliderContrast->setMinimum(MIICAM_CONTRAST_MIN);
    ui->sliderContrast->setMaximum(MIICAM_CONTRAST_MAX);
    ui->sliderContrast->setValue(MIICAM_CONTRAST_DEF);
    ui->editContrast->setPlainText(QString::number(MIICAM_CONTRAST_DEF));

    ui->sliderGamma->setMinimum(MIICAM_GAMMA_MIN);
    ui->sliderGamma->setMaximum(MIICAM_GAMMA_MAX);
    ui->sliderGamma->setValue(MIICAM_GAMMA_DEF);
    ui->editGamma->setPlainText(QString::number(MIICAM_GAMMA_DEF));

    ui->editDarkFieldCorrectionQuantity->setPlainText(QString::number(SimpConstValue::MIICAM_DARK_FIELD_QUANTITY_DEFAULT));

    this->videoWritingDialog->setLabelText("writing video...");
    this->videoWritingDialog->setRange(0, 100);
    this->videoWritingDialog->setModal(true);
    this->videoWritingDialog->reset();
    this->videoWritingDialog->hide();
}

void TabCamera::EnableUI(bool isPlay)
{
    ui->cbResolution->setEnabled(!isPlay);
    ui->cbFormat->setEnabled(!isPlay);

    ui->btnStopCamera->setEnabled(isPlay);
    ui->btnCaptureCamera->setEnabled(isPlay);

    ui->btnRecordOn->setEnabled(isPlay);
    ui->btnRecordOption->setEnabled(isPlay);

    ui->sliderExposureTime->setEnabled(isPlay);
    ui->editExposureTime->setEnabled(isPlay);

    ui->sliderGain->setEnabled(isPlay);
    ui->editGain->setEnabled(isPlay);

    ui->sliderContrast->setEnabled(isPlay);
    ui->editContrast->setEnabled(isPlay);

    ui->sliderGamma->setEnabled(isPlay);
    ui->editGamma->setEnabled(isPlay);

    ui->btnCurveSetting->setEnabled(isPlay);
    ui->cbCurvePreset->setEnabled(isPlay);

    ui->editDarkFieldCorrectionQuantity->setEnabled(isPlay);
    ui->btnDarkFieldCorrection->setEnabled(isPlay);
    ui->chkDarkFieldCorrection->setEnabled(isPlay && this->isDarkFieldCorrectCapture);

    ui->btnZoomIn->setEnabled(isPlay);
    ui->btnZoomOut->setEnabled(isPlay);
}

void TabCamera::cbResoution_SelectedIndexChanged(int index)
{
    this->resolutionIndex = index;
    this->imageWidth = this->miiDevice.model->res[index].width;
    this->imageHeight = this->miiDevice.model->res[index].height;

    if (this->miiHcam) //step 1: stop camera
    {
        Miicam_Stop(this->miiHcam);
    }

    Miicam_put_eSize(this->miiHcam, static_cast<unsigned>(resolutionIndex));

    ui->btnPlayCamera->setEnabled(true);
}

void TabCamera::cbFormat_SelectedIndexChanged(int index)
{
    /* MIICAM_OPTION_RGB
     * 0 => RGB24;
     * 1 => enable RGB48 format when bitdepth > 8;
     * 2 => RGB32;
     * 3 => 8 Bits Grey (only for mono camera);
     * 4 => 16 Bits Grey (only for mono camera when bitdepth > 8);
     * 5 => 64(RGB64)
     *
     * MIICAM_OPTION_RAW
     * 0 = rgb,
     * 1 = raw,
     * default value: 0
    */
    if (index == static_cast<int>(PixelFormatType::RGB24))
    {
        this->imageFormat = QImage::Format_RGB888;
        // Handle RGB24 case
        Miicam_put_Option(miiHcam, MIICAM_OPTION_RGB, 0);
    }
    // else if (index == static_cast<int>(PixelFormatType::RGB32))
    // {
    //     this->imageFormat = QImage::Format_RGB32;
    //     Miicam_put_Option(miiHcam, MIICAM_OPTION_RGB, 2);
    // }
    else if (index == static_cast<int>(PixelFormatType::Raw))
    {
        this->imageFormat = QImage::Format_RGB888;
        Miicam_put_Option(miiHcam, MIICAM_OPTION_RAW, 0);
    }
}

void TabCamera::btnPlayCamera_Click()
{
    // camera가 run이 아니었으면 시작
    if (!this->isCameraRun)
    {
        TabCamera::StartCamera();
        this->isCameraPlay = true;

        ui->btnPlayCamera->setText(SimpConstMenu::BTN_PAUSE);

        TabCamera::EnableUI(true);
    }
    else
    {
        this->isCameraPlay = !this->isCameraPlay;

        if (this->isCameraPlay)
        {
            // resume camera
            Miicam_Pause(this->miiHcam, 0);  /* 1 => pause, 0 => continue */
            ui->btnPlayCamera->setText(SimpConstMenu::BTN_PAUSE);
        }
        else
        {
            // pause camera
            Miicam_Pause(this->miiHcam, 1);  /* 1 => pause, 0 => continue */
            ui->btnPlayCamera->setText(SimpConstMenu::BTN_PLAY);
        }
    }
}

void TabCamera::btnStopCamera_Click()
{
    Miicam_Stop(this->miiHcam);
    this->isCameraRun = false;

    ui->btnPlayCamera->setText(SimpConstMenu::BTN_PLAY);
    TabCamera::EnableUI(false);

    // 명시적으로 false로 만든다
    this->isRawCameraData = false;
}

void TabCamera::btnCaptureCamera_Click()
{
    if (!this->resultFrame.isNull())
    {
        QDir dir(SimpConstPath::DIR_CAPTURE_FRAME);
        if (!dir.exists())
        {
            dir.mkpath(SimpConstPath::DIR_CAPTURE_FRAME);
        }

        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString timestamp = currentDateTime.toString(SimpConstFormat::DATE_TIME);
        QString filePath = dir.absoluteFilePath(timestamp + SimpConstPath::EXTENSION_CAPTURE_IMAGE);

        this->resultFrame.save(filePath);
    }
}

void TabCamera::btnRecordOn_Click()
{
    this->isRecordOn = !this->isRecordOn;

    if (this->isRecordOn)
    {
        TabCamera::StartRecord();
    }
    else
    {
        TabCamera::FinishRecord();
    }
}

void TabCamera::btnRecordOption_Click()
{
    DialogRecordOption dialog(this->recordDir, this->recordFormat, this->recordFrameRate, this->recordQuality, this->recordTimeLimit, this);

    if (dialog.exec() == QDialog::Accepted)
    {
        this->recordDir = dialog.getVideoDirectory();
        this->recordFormat = dialog.getVideoFormat();
        this->recordFrameRate = dialog.getFrameRate();
        this->recordQuality = dialog.getQuality();
        this->recordTimeLimit = dialog.getTimeLimit();

        this->cameraDelay = SimpConstValue::SECOND / this->recordFrameRate;
    }
}

void TabCamera::sliderExposureTime_sliderMoved(int position)
{
    // trackbar가 정수이므로 0.1을 곱한다.
    double value = ui->sliderExposureTime->value() * 0.1;

    // time이 microsecond이고 range가 0.1-50000이기 때문에 0.01을 곱한다.
    Miicam_put_ExpoTime(this->miiHcam, (unsigned int)(value * 100.0));

    // label도 업데이트
    ui->editExposureTime->setPlainText(QString::number(value, 'f', 1));
}

void TabCamera::editExposureTime_editingFinished()
{
    bool ok;
    double value = ui->editExposureTime->toPlainText().toDouble(&ok);

    if (ok)
    {
        // trackbar에는 정수로 들어가야 하므로 10을 곱한다.
        int valueInt = (int)(value * 10.0);

        if (valueInt >= ui->sliderExposureTime->minimum() && valueInt <= ui->sliderExposureTime->maximum())
        {
            // time이 microsecond이고 range가 0.1-50000이기 때문에 0.01을 곱한다.
            Miicam_put_ExpoTime(this->miiHcam, (unsigned int)(value * 100.0));

            // slider에도 값 업데이트
            ui->sliderExposureTime->setValue(valueInt);
        }
        else
        {
            QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            value = SimpUtil::roundToDecimalPlaces(ui->sliderExposureTime->value() * 0.1, 1);
            ui->editExposureTime->setPlainText(QString::number(value, 'f', 1));
        }
    }
    else
    {
        QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        value = SimpUtil::roundToDecimalPlaces(ui->sliderExposureTime->value() * 0.1, 1);
        ui->editExposureTime->setPlainText(QString::number(value, 'f', 1));
    }
}

void TabCamera::sliderGain_sliderMoved(int position)
{
    int value = ui->sliderGain->value();

    Miicam_put_ExpoAGain(this->miiHcam, (unsigned short)(value));

    // label도 업데이트
    ui->editGain->setPlainText(QString::number(value));
}

void TabCamera::editGain_editingFinished()
{
    bool ok;
    int value = ui->editGain->toPlainText().toInt(&ok);

    if (ok)
    {
        if (value >= ui->sliderGain->minimum() && value <= ui->sliderGain->maximum())
        {
            Miicam_put_ExpoAGain(this->miiHcam, (unsigned short)(value));

            // slider에도 값 업데이트
            ui->sliderGain->setValue(value);
        }
        else
        {
            QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            ui->editGain->setPlainText(QString::number(ui->sliderGain->value()));
        }
    }
    else
    {
        QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        ui->editGain->setPlainText(QString::number(ui->sliderGain->value()));
    }
}

void TabCamera::sliderContrast_sliderMoved(int position)
{
    int value = ui->sliderContrast->value();

    Miicam_put_Contrast(this->miiHcam, value);

    // label도 업데이트
    ui->editContrast->setPlainText(QString::number(value));
}

void TabCamera::editContrast_editingFinished()
{
    bool ok;
    int value = ui->editContrast->toPlainText().toInt(&ok);

    if (ok)
    {
        if (value >= ui->sliderContrast->minimum() && value <= ui->sliderContrast->maximum())
        {
            Miicam_put_Contrast(this->miiHcam, value);

            // slider에도 값 업데이트
            ui->sliderContrast->setValue(value);
        }
        else
        {
            QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            ui->editContrast->setPlainText(QString::number(ui->sliderContrast->value()));
        }
    }
    else
    {
        QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        ui->editContrast->setPlainText(QString::number(ui->sliderContrast->value()));
    }
}

void TabCamera::sliderGamma_sliderMoved(int position)
{
    int value = ui->sliderGamma->value();

    Miicam_put_Gamma(this->miiHcam, value);

    // label도 업데이트
    ui->editGamma->setPlainText(QString::number(value));
}

void TabCamera::editGamma_editingFinished()
{
    bool ok;
    int value = ui->editGamma->toPlainText().toInt(&ok);

    if (ok)
    {
        if (value >= ui->sliderGamma->minimum() && value <= ui->sliderGamma->maximum())
        {
            Miicam_put_Gamma(this->miiHcam, value);

            // slider에도 값 업데이트
            ui->sliderGamma->setValue(value);
        }
        else
        {
            QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            ui->editGamma->setPlainText(QString::number(ui->sliderGamma->value()));
        }
    }
    else
    {
        QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        ui->editGamma->setPlainText(QString::number(ui->sliderGamma->value()));
    }
}

void TabCamera::btnCurveSetting_Click()
{
    DialogContrastCurve dialog(this->presetsContrastCurve, ui->cbCurvePreset->currentIndex() - 1, this->isUpdateContrastCurve, this);
    connect(&dialog, &DialogContrastCurve::contrastCurveUpdated, this, &TabCamera::UpdateContrastCurvePoints);

    if (dialog.exec() == QDialog::Accepted)
    {
        this->presetsContrastCurve = dialog.getPresets();
        this->isUpdateContrastCurve = dialog.getEnable();

        QJsonArray jsonArray;
        SimpUtil::convertPresetsImageCurveToJsonArray(this->presetsContrastCurve, jsonArray);

        QString pathPreset = SimpConstPath::PATH_JSON_CONTRAST_CURVE;
        SimpUtil::saveJsonFile(pathPreset, jsonArray);

        // 여기에는 none이 있기 때문에 index에 +1 해야 함.
        int index = this->isUpdateContrastCurve ? dialog.getSelectedIndex() + 1 : 0;
        TabCamera::UpdatePresetContrastCurve(this->presetsContrastCurve, index);
    }
    else
    {
        int index = ui->cbCurvePreset->currentIndex();

        // 선택된 preset이 있었으면 그것으로 업데이트
        if (index > 0)
        {
            TabCamera::UpdateContrastCurvePoints(this->presetsContrastCurve[index - 1].GetPoints());
        }
        // none이면 update 안 함.
        else
        {
            this->isUpdateContrastCurve = false;
        }
    }
}

void TabCamera::cbCurvePreset_SelectedIndexChanged(int index)
{
    // 0이면 none
    if (index > 0)
    {
        PresetContrastCurve preset = this->presetsContrastCurve[index - 1];
        TabCamera::UpdateContrastCurvePoints(preset.GetPoints());
    }
    else
    {
        this->isUpdateContrastCurve = false;
    }
}

void TabCamera::chkDarkFieldCorrection_CheckedChanged(Qt::CheckState checkState)
{
    int option = ui->chkDarkFieldCorrection->isChecked() ? 1 : 0;
    Miicam_put_Option(this->miiHcam, MIICAM_OPTION_DFC, option);
}

void TabCamera::btnDarkFieldCorrection_Click()
{
    bool ok;
    int value = ui->editDarkFieldCorrectionQuantity->toPlainText().toInt(&ok);

    if (ok)
    {
        if (value >= SimpConstValue::MIICAM_DARK_FIELD_QUANTITY_MIN && value <= SimpConstValue::MIICAM_DARK_FIELD_QUANTITY_MAX)
        {
            int optionValue = 0xff000000 | value;
            Miicam_put_Option(this->miiHcam, MIICAM_OPTION_DFC, optionValue);

            // Dark Filed Correction 적용
            Miicam_DfcOnce(this->miiHcam);

            this->isDarkFieldCorrectCapture = true;
            ui->chkDarkFieldCorrection->setEnabled(this->isDarkFieldCorrectCapture);
        }
        else
        {
            QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_RANGE);
            ui->editGamma->setPlainText(QString::number(SimpConstValue::MIICAM_DARK_FIELD_QUANTITY_DEFAULT));
        }
    }
    else
    {
        QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_VALUE);
        ui->editGamma->setPlainText(QString::number(SimpConstValue::MIICAM_DARK_FIELD_QUANTITY_DEFAULT));
    }
}

void TabCamera::btnGroupCooling_Click(int id)
{
    bool enable = id == 1;
    ui->sliderTemperature->setEnabled(enable);
    ui->editTemperature->setEnabled(enable);
}

void TabCamera::sliderTemperature_sliderMoved(int position)
{
    // trackbar가 정수이므로 0.1을 곱한다.
    double value = ui->sliderTemperature->value() * 0.1;

    // temperature는 3.2도를 32로 받기 때문에 10을 곱한다.
    Miicam_put_Temperature(this->miiHcam, (short)(value * 10.0));

    // label도 업데이트
    ui->editTemperature->setPlainText(QString::number(value, 'f', 1));
}

void TabCamera::editTemperature_editingFinished()
{
    bool ok;
    double value = ui->editTemperature->toPlainText().toDouble(&ok);

    if (ok)
    {
        // trackbar에는 정수로 들어가야 하므로 10을 곱한다.
        int valueInt = (int)(value * 10.0);

        if (valueInt >= ui->sliderTemperature->minimum() && valueInt <= ui->sliderTemperature->maximum())
        {
            // temperature는 3.2도를 32로 받기 때문에 10을 곱한다.
            Miicam_put_Temperature(this->miiHcam, (short)(value * 10.0));

            // slider에도 값 업데이트
            ui->sliderTemperature->setValue(valueInt);
        }
        else
        {
            QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            value = SimpUtil::roundToDecimalPlaces(ui->sliderTemperature->value() * 0.1, 1);
            ui->editTemperature->setPlainText(QString::number(value, 'f', 1));
        }
    }
    else
    {
        QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        value = SimpUtil::roundToDecimalPlaces(ui->sliderTemperature->value() * 0.1, 1);
        ui->editTemperature->setPlainText(QString::number(value, 'f', 1));
    }
}

void TabCamera::btnZoomIn_Click()
{
    this->zoomFactor += SimpConstValue::ZOOM_VALUE;

    if (this->zoomFactor > SimpConstValue::ZOOM_MAX)
    {
        this->zoomFactor = SimpConstValue::ZOOM_MAX;
    }

    // 현재 창 크기 기준으로 zoom 조절. 실제 이미지 크기로 하면 창 크기를 넘어선다.
    ui->gvCamera->fitInView();
    ui->gvCamera->scale(this->zoomFactor, this->zoomFactor);

    ui->lbZoom->setText(QString("Zoom x%1").arg(this->zoomFactor, 0, 'f', 2));

}

void TabCamera::btnZoomOut_Click()
{
    this->zoomFactor -= SimpConstValue::ZOOM_VALUE;

    if (this->zoomFactor < SimpConstValue::ZOOM_MIN)
    {
        this->zoomFactor = SimpConstValue::ZOOM_MIN;
    }

    // 현재 창 크기 기준으로 zoom 조절. 실제 이미지 크기로 하면 창 크기를 넘어선다.
    ui->gvCamera->fitInView();
    ui->gvCamera->scale(this->zoomFactor, this->zoomFactor);

    ui->lbZoom->setText(QString("Zoom x%1").arg(this->zoomFactor, 0, 'f', 2));
}

void TabCamera::UpdateMousePosition(int x, int y, const QColor &color)
{
    QString text = QString("(x: %1, y: %2), (r: %3, g: %4, b: %5)")
                       .arg(x)
                       .arg(y)
                       .arg(color.red())
                       .arg(color.green())
                       .arg(color.blue());
    ui->lbCameraPixel->setText(text);  // Assume you have a QLabel named label in your .ui file
}

///////////////////////////////////////////////// camera
void TabCamera::UpdatePreview()
{
    while (this->isCameraOn)
    {
        if (this->isCameraPlay && this->isRawCameraData)
        {
            QImage source(this->rawCameraData, this->rawCameraWidth, this->rawCameraHeight, this->imageFormat);

            try
            {
                // gegl에서는 rgba를 받기 때문에 무조건 rgba로 바꿔야 한다.
                QImage formattedSource = source.convertToFormat(QImage::Format_RGBA8888);

                if (this->isUpdateContrastCurve)
                {
                    SimpGEGL::UpdateContrastCurve(formattedSource, this->contrastCurvePoints);
                }

                // gegl을 적용한 후에 result에 넣는다. 그래야 video나 capture에서 gegl이 적용된 이미지가 사용될 수 있음.
                this->resultFrame = formattedSource.convertToFormat(QImage::Format_RGB888);

                if (this->isRecordOn)
                {
                    cv::Mat mat(this->resultFrame.height(), this->resultFrame.width(), CV_8UC3, const_cast<uchar*>(this->resultFrame.bits()), this->resultFrame.bytesPerLine());
                    cv::Mat matBGR;
                    cv::cvtColor(mat, matBGR, cv::COLOR_RGB2BGR);  // rgb -> bgr

                    this->recordFrames.emplace_back(matBGR);
                }

                QMetaObject::invokeMethod(this, "UpdatePreviewUI", Qt::QueuedConnection);
            }
            catch(const std::exception &e)
            {
                SimpLogger::Log(e.what());
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(this->cameraDelay)); // Frame rate delay
    }
}

void TabCamera::UpdatePreviewUI()
{
    ui->gvCamera->setImage(this->resultFrame);

    if (std::abs(this->zoomFactor - 1.0f) <= std::numeric_limits<float>::epsilon())
    {
        ui->gvCamera->fitInView();
    }

    if (this->isRecordOn)
    {
        QTime currentTime = QTime::currentTime();
        int elapsedSeconds = this->recordStartTime.secsTo(currentTime);
        QTime elapsedTime(0, 0);
        elapsedTime = elapsedTime.addSecs(elapsedSeconds);

        ui->lbRecordingTime->setText("Recoding Time: " + elapsedTime.toString("hh:mm:ss"));

        if (this->recordTimeLimit > 0 && elapsedTime.second() >= this->recordTimeLimit)
        {
            this->isRecordOn = false;
            TabCamera::FinishRecord();
        }
    }
}

void TabCamera::StartRecord()
{
    ui->btnRecordOn->setText(SimpConstMenu::BTN_RECORD_OFF);

    this->recordFrames.clear();
    this->recordStartTime = QTime::currentTime();
    this->timerVideoRecord->start(1000); // Update every second
}

void TabCamera::FinishRecord()
{
    if (this->recordFrames.size() > 0)
    {
        QDir dir(this->recordDir);
        if (!dir.exists())
        {
            dir.mkpath(this->recordDir);
        }

        QString timestamp = QDateTime::currentDateTime().toString(SimpConstFormat::DATE_TIME);
        QString filePath = dir.absoluteFilePath(timestamp + SimpUtil::getVideoExtension(this->recordFormat));        

        QTime currentTime = QTime::currentTime();
        int elapsedSeconds = this->recordStartTime.secsTo(currentTime);

        this->videoWritingDialog->reset();
        this->videoWritingDialog->show();

        WorkerVideoWritingCV *writer = new WorkerVideoWritingCV(
            this->recordFrames
            , /*format*/this->recordFormat
            , /*recordSecond*/elapsedSeconds
            , /*quality*/this->recordQuality
            , /*filePath*/filePath
            );

        connect(writer, &WorkerVideoWritingCV::progress, this, &TabCamera::onVideoWritingProgress);
        connect(writer, &WorkerVideoWritingCV::cancelled, this, &TabCamera::onVideoWritingCanceled);
        connect(writer, &WorkerVideoWritingCV::finished, this, &TabCamera::onVideoWritingFinished);
        connect(writer, &WorkerVideoWritingCV::finished, writer, &QObject::deleteLater);  // QObject::deleteLater가 thread를 제거함

        // dialog 취소 버튼 클릭하면 converting 중지
        connect(this->videoWritingDialog, &QProgressDialog::canceled, this, [=]() {
            writer->requestInterruption();
        });

        writer->start();
    }
}

void TabCamera::UpdatePresetContrastCurve(const std::vector<PresetContrastCurve>& presets, const int index)
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

void TabCamera::LoadPresets()
{
    QJsonArray jsonArray;

    if (SimpUtil::loadJsonFile(SimpConstPath::PATH_JSON_CONTRAST_CURVE, jsonArray))
    {
        this->presetsContrastCurve = SimpUtil::convertJsonToPresetsImageCurve(jsonArray);
    }
}

void TabCamera::UpdateContrastCurvePoints(const QVector<QPointF>& points)
{
    this->contrastCurvePoints = points;
    this->isUpdateContrastCurve = true;
}

void TabCamera::onVideoWritingProgress(int current, int total)
{
    int value = ((current + 1) * 100) / total;
    this->videoWritingDialog->setValue(value);
}

void TabCamera::onVideoWritingCanceled()
{
    this->videoWritingDialog->hide(); // Hide the progress dialog
    QMessageBox::information(this, "Cancel", "Canceled Video Writing.");

    this->recordFrames.clear();
    ui->btnRecordOn->setText(SimpConstMenu::BTN_RECORD_ON);

    // 처리가 완료 되었으므로 enable
    TabCamera::EnableUI(true);
}

void TabCamera::onVideoWritingFinished(bool success)
{
    this->videoWritingDialog->hide(); // Hide the progress dialog

    if (success)
    {
        this->recordFrames.clear();
        ui->btnRecordOn->setText(SimpConstMenu::BTN_RECORD_ON);

        // 처리가 완료 되었으므로 enable
        TabCamera::EnableUI(true);
    }
    else
    {
        QMessageBox::warning(this, "Error", "Could not write the video file.");
    }
}


///////////////////////////////////////////////// mii cam
void TabCamera::FindCamera()
{
    if (this->miiHcam)
    {
        CloseCamera();
    }
    else
    {
        MiicamDeviceV2 arr[MIICAM_MAX] = { 0 };
        unsigned count = Miicam_EnumV2(arr);
        if (count > 0)
        {
            this->miiDevice = arr[0];

            {
                const QSignalBlocker blocker(ui->lbDeviceName);
                ui->lbDeviceName->setText(QString::fromWCharArray(this->miiDevice.displayname));
            }

            TabCamera::OpenCamera();
        }
        else
        {
            QMessageBox::warning(this, "Warning", "No camera found.");
        }
    }
}

void TabCamera::OpenCamera()
{
    this->miiHcam = Miicam_Open(this->miiDevice.id);

    if (this->miiHcam)
    {
        Miicam_get_eSize(this->miiHcam, (unsigned*)&this->resolutionIndex);
        this->imageWidth = this->miiDevice.model->res[this->resolutionIndex].width;
        this->imageHeight = this->miiDevice.model->res[this->resolutionIndex].height;

        Miicam_put_Option(this->miiHcam, MIICAM_OPTION_BYTEORDER, 0); //Qimage use RGB byte order
        Miicam_put_AutoExpoEnable(this->miiHcam, 0);  // auto exposure disable

        // open에 성공하면 resolution 업데이트
        {
            const QSignalBlocker blocker(ui->cbResolution);

            TabCamera::InitCameraResolution();
            TabCamera::InitSensorTemperature();

            // open에 성공하면 true
            ui->cbResolution->setEnabled(true);
            ui->cbFormat->setEnabled(true);
            ui->btnPlayCamera->setEnabled(true);
            ui->rbCoolingOn->setEnabled(true);
            ui->rbCoolingOff->setEnabled(true);
            ui->sliderTemperature->setEnabled(true);
            ui->editTemperature->setEnabled(true);

            ui->btnPlayCamera->setText(SimpConstMenu::BTN_PLAY);
            ui->rbCoolingOn->setChecked(true);
        }

    }
}

void TabCamera::CloseCamera()
{
    ui->btnPlayCamera->setEnabled(false);
    ui->btnPlayCamera->setText(SimpConstMenu::BTN_PLAY);

    if (this->miiHcam)
    {
        Miicam_Close(this->miiHcam);
        this->miiHcam = nullptr;
    }

    if (this->rawCameraData)
    {
        delete[] this->rawCameraData;
        this->rawCameraData = nullptr;
    }
}

void TabCamera::StartCamera()
{
    if (this->rawCameraData)
    {
        delete[] this->rawCameraData;
        this->rawCameraData = nullptr;
    }

    this->rawCameraData = new uchar[TDIBWIDTHBYTES(imageWidth * 24) * this->imageHeight];

    TabCamera::UpdateExposureTime();

    // if ((this->miiDevice.model->flag & MIICAM_FLAG_MONO) == 0)
    // {
    //     handleTempTintEvent();
    // }

    // handleExpoEvent();

    if (SUCCEEDED(Miicam_StartPullModeWithCallback(this->miiHcam, eventCallBack, this)))
    {
        this->isCameraRun = true;

        // fps update
        this->timerFPS->start(1000);
    }
    else
    {
        CloseCamera();
        QMessageBox::warning(this, "Warning", "Failed to start camera.");
    }
}

void TabCamera::InitCameraResolution()
{
    ui->cbResolution->clear();
    for (unsigned i = 0; i < this->miiDevice.model->preview; ++i)
    {
        ui->cbResolution->addItem(QString::asprintf("%u x %u", this->miiDevice.model->res[i].width, this->miiDevice.model->res[i].height));
    }
    ui->cbResolution->setCurrentIndex(this->resolutionIndex);
}

void TabCamera::UpdateExposureTime()
{
    unsigned int nMin, nMax, nDef, nTime;
    Miicam_get_ExpTimeRange(this->miiHcam, &nMin, &nMax, &nDef);
    Miicam_get_ExpoTime(this->miiHcam, &nTime);

    double min = nMin * 0.01;
    double max = nMax * 0.01;
    double def = nDef * 0.01;
    double value = nTime * 0.01;

    if (min < SimpConstValue::MIICAM_EXPOSURE_TIME_MIN)
    {
        min = SimpConstValue::MIICAM_EXPOSURE_TIME_MIN;
    }

    if (max > SimpConstValue::MIICAM_EXPOSURE_TIME_MAX)
    {
        max = SimpConstValue::MIICAM_EXPOSURE_TIME_MAX;
    }

    ui->sliderExposureTime->setMinimum((int)(min * 10.0));
    ui->sliderExposureTime->setMaximum((int)(max * 10.0));
    ui->sliderExposureTime->setValue((int)(value * 10.0));
    ui->editExposureTime->setPlainText(QString::number(value, 'f', 1));
}

void TabCamera::InitSensorTemperature()
{
    // 강제로 -50도로 초기화
    double value = SimpConstValue::MIICAM_TEMPERATURE_MIN * 10.0;
    Miicam_put_Temperature(this->miiHcam, (short)value);

    short temperature;
    Miicam_get_Temperature(this->miiHcam, &temperature);

    // temperature는 3.2도를 32로 받기 때문에 0.1을 곱한다.
    //double value = temperature * 0.1;

    ui->sliderTemperature->setMinimum((int)(SimpConstValue::MIICAM_TEMPERATURE_MIN * 10.0));
    ui->sliderTemperature->setMaximum((int)(SimpConstValue::MIICAM_TEMPERATURE_MAX * 10.0));
    ui->sliderTemperature->setValue((int)value);
    ui->editTemperature->setPlainText(QString::number(value * 0.1, 'f', 1));
}

void TabCamera::eventCallBack(unsigned nEvent, void* pCallbackCtx)
{
    TabCamera* pThis = reinterpret_cast<TabCamera*>(pCallbackCtx);
    emit pThis->evtCallback(nEvent);
}

void TabCamera::onTimerFpsCallback()
{
    unsigned nFrame = 0, nTime = 0, nTotalFrame = 0;

    if (this->miiHcam && SUCCEEDED(Miicam_get_FrameRate(this->miiHcam, &nFrame, &nTime, &nTotalFrame)) && (nTime > 0))
    {
        ui->lbCameraFPS->setText(QString::asprintf("%u, fps = %.1f", nTotalFrame, nFrame * 1000.0 / nTime));
    }
}

void TabCamera::onMiiCameraCallback(unsigned nEvent)
{
    /* this run in the UI thread */
    if (this->miiHcam)
    {
        if (MIICAM_EVENT_IMAGE == nEvent)
        {
            handleImageEvent();
        }
        // auto exposure를 사용하지 않기 때문에 안 함
        //else if (MIICAM_EVENT_EXPOSURE == nEvent)
        //{
        //    handleExpoEvent();
        //}
        // 여기서 temperature는 색온도를 의미하는 것으로 장치의 온도가 아니다. tint는 색조. 둘 다 사용 안 함.
        //else if (MIICAM_EVENT_TEMPTINT == nEvent)
        //{
        //    handleTempTintEvent();
        //}
        // capture는 따로 하기 때문에 처리 안 함
        //else if (MIICAM_EVENT_STILLIMAGE == nEvent)
        //{
        //    handleStillImageEvent();
        //}
        else if (MIICAM_EVENT_ERROR == nEvent)
        {
            CloseCamera();
            QMessageBox::warning(this, "Warning", "Generic error.");
        }
        else if (MIICAM_EVENT_DISCONNECTED == nEvent)
        {
            CloseCamera();
            QMessageBox::warning(this, "Warning", "Camera disconnect.");
        }
    }
}

void TabCamera::handleImageEvent()
{
    QMutexLocker locker(&imageMutex);

    // camera의 이미지를 받아서 rawCameraData에 담는다.
    this->isRawCameraData = SUCCEEDED(Miicam_PullImage(this->miiHcam, this->rawCameraData, 24, &this->rawCameraWidth, &this->rawCameraHeight));
}
