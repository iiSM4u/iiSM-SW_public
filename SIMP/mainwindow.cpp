#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include "dialog_record_option.h"
#include "dialog_brightness_contrast.h"
#include "dialog_stretch_contrast.h"
#include "dialog_stress.h"
#include "dialog_contrast_curve.h"
#include "videoloader.h"

#include <QGraphicsPixmapItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QImage>
#include <QBuffer>
#include <QFile>
#include <QProcessEnvironment>
#include <QDebug>
#include <QCoreApplication>
#include <QDebug>
#include <QLibrary>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QPlainTextEdit>
#include <thread>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , progressDialog(new QProgressDialog(this))
    , filesystemVideo(new QFileSystemModel(this))
    , filesystemFrame(new QFileSystemModel(this))
    , timerFPS(new QTimer(this))
    , timerVideoRecord(new QTimer(this))
    , btnGroupCooling(new QButtonGroup(this))
    , recordDir(QCoreApplication::applicationDirPath() + DIR_RECORD_VIDEO)
    , captureDir(QCoreApplication::applicationDirPath() + DIR_CAPTURE_FRAME)
{
    ui->setupUi(this);

    qApp->setStyleSheet(
        "QPlainTextEdit:disabled { background-color: lightgray; color: darkgray; }"
        "QPushButton:disabled { background-color: lightgray; color: darkgray; }"
        "QComboBox:disabled { background-color: lightgray; color: darkgray; }"
        "QCheckBox:disabled { color: darkgray; }"
        "QSlider:disabled { color: darkgray; }"
        "QRadioButton:disabled { color: darkgray; }"
    );

    // ui init 전에 preset load를 해야 combobox를 채울 수 있다.
    MainWindow::LoadPresets();

    MainWindow::ConnectUI();
    MainWindow::InitUI();
    MainWindow::UpdatePresetContrastCurve(this->presetsContrastCurve);

    // 일단 false로 시작
    MainWindow::EnablePreviewUI(false);

    ui->sliderTemperature->setEnabled(false);
    ui->editTemperature->setEnabled(false);

    // thread 시작
    this->isOn = true;
    threadPreview = std::thread(&MainWindow::UpdatePreview, this);
    threadVideo = std::thread(&MainWindow::UpdateVideo, this);

    MainWindow::InitGegl();

    // ui 초기화 후에 우선 카메라부터 찾는다.
    MainWindow::FindCamera();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent*)
{
    this->isOn = false;

    if (threadPreview.joinable()) {
        threadPreview.join();
    }

    if (threadVideo.joinable()) {
        threadVideo.join();
    }

    MainWindow::CloseGegl();
    MainWindow::CloseCamera();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (ui->tabWidget->currentWidget() == ui->tabVideo)
    {
        // play 중일 떄는 play하면서 업데이트하므로 하지 않는다.
        if (!this->isVideoPlay && std::abs(this->zoomFactor - 1.0f) <= std::numeric_limits<float>::epsilon())
        {
            ui->gvVideo->fitInView();
        }
    }    
    else if (ui->tabWidget->currentWidget() == ui->tabCapture)
    {
        ui->gvFrame->fitInView();
    }
}


////////////////////////////////////////
// UI
////////////////////////////////////////
void MainWindow::ConnectUI()
{
    connect(this, &MainWindow::evtCallback, this, &MainWindow::onMiiCameraCallback);
    connect(timerFPS, &QTimer::timeout, this, &MainWindow::onTimerFpsCallback);

    ///////////////////////////////// preview
    connect(ui->gvPreview, &CustomGraphicsView::mousePositionChanged, this, &MainWindow::UpdatePreviewMousePosition);
    connect(ui->gvVideo, &CustomGraphicsView::mousePositionChanged, this, &MainWindow::UpdateVideoMousePosition);
    connect(ui->gvFrame, &CustomGraphicsView::mousePositionChanged, this, &MainWindow::UpdateFrameMousePosition);

    connect(ui->cbResolution, &QComboBox::currentIndexChanged, this, &MainWindow::cbResoution_SelectedIndexChanged);
    connect(ui->cbFormat, &QComboBox::currentIndexChanged, this, &MainWindow::cbFormat_SelectedIndexChanged);

    connect(ui->btnPlayCamera, &QPushButton::clicked, this, &MainWindow::btnPlayCamera_Click);
    connect(ui->btnStopCamera, &QPushButton::clicked, this, &MainWindow::btnStopCamera_Click);
    connect(ui->btnCaptureCamera, &QPushButton::clicked, this, &MainWindow::btnCaptureCamera_Click);
    connect(ui->btnRecordOn, &QPushButton::clicked, this, &MainWindow::btnRecordOn_Click);
    connect(ui->btnRecordOption, &QPushButton::clicked, this, &MainWindow::btnRecordOption_Click);

    connect(ui->sliderExposureTime, &QSlider::sliderMoved, this, &MainWindow::sliderExposureTime_sliderMoved);
    connect(ui->editExposureTime, &CustomPlainTextEdit::editingFinished, this, &MainWindow::editExposureTime_editingFinished);

    connect(ui->sliderGain, &QSlider::sliderMoved, this, &MainWindow::sliderGain_sliderMoved);
    connect(ui->editGain, &CustomPlainTextEdit::editingFinished, this, &MainWindow::editGain_editingFinished);

    connect(ui->sliderContrast, &QSlider::sliderMoved, this, &MainWindow::sliderContrast_sliderMoved);
    connect(ui->editContrast, &CustomPlainTextEdit::editingFinished, this, &MainWindow::editContrast_editingFinished);

    connect(ui->sliderGamma, &QSlider::sliderMoved, this, &MainWindow::sliderGamma_sliderMoved);
    connect(ui->editGamma, &CustomPlainTextEdit::editingFinished, this, &MainWindow::editGamma_editingFinished);

    connect(ui->btnCurveSetting, &QPushButton::clicked, this, &MainWindow::btnCurveSetting_Click);
    connect(ui->cbCurvePreset, &QComboBox::currentIndexChanged, this, &MainWindow::cbCurvePreset_SelectedIndexChanged);

    connect(ui->chkDarkFieldCorrection, &QCheckBox::checkStateChanged, this, &MainWindow::chkDarkFieldCorrection_CheckedChanged);
    connect(ui->btnDarkFieldCorrection, &QPushButton::clicked, this, &MainWindow::btnDarkFieldCorrection_Click);

    // off가 클릭된 상태로 시작
    ui->rbCoolingOff->setChecked(true);
    this->btnGroupCooling->addButton(ui->rbCoolingOn, 1);
    this->btnGroupCooling->addButton(ui->rbCoolingOff, 2);
    connect(this->btnGroupCooling, &QButtonGroup::idClicked, this, &MainWindow::btnGroupCooling_Click);

    connect(ui->sliderTemperature, &QSlider::sliderMoved, this, &MainWindow::sliderTemperature_sliderMoved);
    connect(ui->editTemperature, &CustomPlainTextEdit::editingFinished, this, &MainWindow::editTemperature_editingFinished);

    connect(ui->btnZoomIn, &QPushButton::clicked, this, &MainWindow::btnZoomIn_Click);
    connect(ui->btnZoomOut, &QPushButton::clicked, this, &MainWindow::btnZoomOut_Click);

    connect(ui->btnBrightnessContrast, &QPushButton::clicked, this, &MainWindow::btnBrightnessContrast_Click);
    connect(ui->btnStress, &QPushButton::clicked, this, &MainWindow::btnStress_Click);
    connect(ui->btnStretchContrast, &QPushButton::clicked, this, &MainWindow::btnStretchContrast_Click);


    ///////////////////////////////// video
    connect(ui->btnLoadVideo, &QPushButton::clicked, this, &MainWindow::btnLoadVideo_Click);
    connect(ui->btnPlayVideo, &QPushButton::clicked, this, &MainWindow::btnPlayVideo_Click);
    connect(ui->btnStopVideo, &QPushButton::clicked, this, &MainWindow::btnStopVideo_Click);
    connect(ui->lvVideo, &QListView::clicked, this, &MainWindow::lvVideo_Click);
    connect(ui->sliderVideo, &QSlider::sliderMoved, this, &MainWindow::sliderVideo_sliderMoved);


    ///////////////////////////////// frame
    connect(ui->btnLoadFrame, &QPushButton::clicked, this, &MainWindow::btnLoadFrame_Click);
    connect(ui->lvFrame, &QListView::clicked, this, &MainWindow::lvFrame_Click);
}

void MainWindow::InitUI()
{
    ////////////////////////// tab preview
    ui->cbFormat->clear();
    for (const auto& format : {PixelFormatType::RGB24, PixelFormatType::Raw})
        //for (const auto& format : {PixelFormatType::RGB24, PixelFormatType::RGB32, PixelFormatType::Raw})
    {
        ui->cbFormat->addItem(toString(format));
    }
    ui->cbFormat->setCurrentIndex(0);

    // init gain, contrast, gamma
    ui->sliderGain->setMinimum(MIICAM_EXPOGAIN_MIN);
    ui->sliderGain->setMaximum(MIICAM_EXPOGAIN_MAX);
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

    ui->editDarkFieldCorrectionQuantity->setPlainText(QString::number(MIICAM_DARK_FIELD_QUANTITY_DEFAULT));


    ////////////////////////// tab video
    // Check if the captures directory exists, and create it if it doesn't
    QDir dirVideo(this->recordDir);
    if (!dirVideo.exists()) {
       dirVideo.mkpath(this->recordDir);
    }

    // Set model properties
    this->filesystemVideo->setRootPath(this->recordDir);
    this->filesystemVideo->setNameFilters(QStringList() << "*.avi" << "*.mp4" << "*.wmv");
    this->filesystemVideo->setNameFilterDisables(false);

    ui->lvVideo->setModel(this->filesystemVideo);
    ui->lvVideo->setRootIndex(this->filesystemVideo->index(this->recordDir)); // Set the root index
    ui->lbDirVideo->setText(this->recordDir);

    ui->btnPlayVideo->setEnabled(false);
    ui->btnStopVideo->setEnabled(false);
    ui->sliderVideo->setEnabled(false);


    ////////////////////////// tab frame
    QDir dirFrame(this->captureDir);
    if (!dirFrame.exists()) {
        dirFrame.mkpath(this->captureDir);
    }

    // Set model properties
    this->filesystemFrame->setRootPath(this->captureDir);
    this->filesystemFrame->setNameFilters(QStringList() << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp" << "*.gif");
    this->filesystemFrame->setNameFilterDisables(false);

    ui->lvFrame->setModel(this->filesystemFrame);
    ui->lvFrame->setRootIndex(this->filesystemFrame->index(this->captureDir)); // Set the root index
    ui->lbDirFrames->setText(this->captureDir);


    ////////////////////////// progressDialog
    progressDialog->setLabelText("Loading video...");
    progressDialog->setCancelButton(nullptr);
    progressDialog->setRange(0, 100);
    progressDialog->setModal(true);
    progressDialog->reset();
}

void MainWindow::EnablePreviewUI(bool isPlay)
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

    ui->rbCoolingOn->setEnabled(isPlay);
    ui->rbCoolingOff->setEnabled(isPlay);
;
    ui->editDarkFieldCorrectionQuantity->setEnabled(isPlay);
    ui->btnDarkFieldCorrection->setEnabled(isPlay);
    ui->chkDarkFieldCorrection->setEnabled(isPlay && this->isDarkFieldCorrectCapture);

    ui->btnBrightnessContrast->setEnabled(isPlay);
    ui->btnStress->setEnabled(isPlay);
    ui->btnStretchContrast->setEnabled(isPlay);
}

/////////////////////// preview
void MainWindow::UpdatePreviewMousePosition(int x, int y, const QColor &color)
{
    if (ui->tabWidget->currentWidget() == ui->tabPreview)
    {
        QString text = QString("(x: %1, y: %2), (r: %3, g: %4, b: %5)")
                           .arg(x)
                           .arg(y)
                           .arg(color.red())
                           .arg(color.green())
                           .arg(color.blue());
        ui->lbColor->setText(text);  // Assume you have a QLabel named label in your .ui file
    }
}

void MainWindow::UpdateVideoMousePosition(int x, int y, const QColor &color)
{
    if (ui->tabWidget->currentWidget() == ui->tabVideo)
    {
        QString text = QString("(x: %1, y: %2), (r: %3, g: %4, b: %5)")
                           .arg(x)
                           .arg(y)
                           .arg(color.red())
                           .arg(color.green())
                           .arg(color.blue());
        ui->lbColor->setText(text);  // Assume you have a QLabel named label in your .ui file
    }
}

void MainWindow::UpdateFrameMousePosition(int x, int y, const QColor &color)
{
    if (ui->tabWidget->currentWidget() == ui->tabCapture)
    {
        QString text = QString("(x: %1, y: %2), (r: %3, g: %4, b: %5)")
                           .arg(x)
                           .arg(y)
                           .arg(color.red())
                           .arg(color.green())
                           .arg(color.blue());
        ui->lbColor->setText(text);  // Assume you have a QLabel named label in your .ui file
    }
}

void MainWindow::cbResoution_SelectedIndexChanged(int index)
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

void MainWindow::cbFormat_SelectedIndexChanged(int index)
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

void MainWindow::btnPlayCamera_Click()
{
    // camera가 run이 아니었으면 시작
    if (!this->isCameraRun)
    {
        MainWindow::StartCamera();
        this->isCameraPlay = true;

        ui->btnPlayCamera->setText(BTN_PAUSE);

        MainWindow::EnablePreviewUI(true);
    }
    else
    {
        this->isCameraPlay = !this->isCameraPlay;

        if (this->isCameraPlay)
        {
            // resume camera
            Miicam_Pause(this->miiHcam, 0);  /* 1 => pause, 0 => continue */
            ui->btnPlayCamera->setText(BTN_PAUSE);
        }
        else
        {
            // pause camera
            Miicam_Pause(this->miiHcam, 1);  /* 1 => pause, 0 => continue */
            ui->btnPlayCamera->setText(BTN_PLAY);
        }
    }
}

void MainWindow::btnStopCamera_Click()
{
    Miicam_Stop(this->miiHcam);
    this->isCameraRun = false;

    ui->btnPlayCamera->setText(BTN_PLAY);
    MainWindow::EnablePreviewUI(false);
}

void MainWindow::btnCaptureCamera_Click()
{
    if (!this->resultPreview.isNull())
    {
        QString pathDir = QCoreApplication::applicationDirPath() + DIR_CAPTURE_FRAME;

        QDir dir(pathDir);
        if (!dir.exists())
        {
            dir.mkpath(pathDir);
        }

        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString timestamp = currentDateTime.toString(FORMAT_DATE_TIME);
        QString filePath = dir.absoluteFilePath(timestamp + EXTENSION_CAPTURE_IMAGE);

        this->resultPreview.save(filePath);
    }
}

void MainWindow::btnRecordOn_Click()
{
    this->isRecordOn = !this->isRecordOn;
    ui->btnRecordOn->setText(this->isRecordOn ? BTN_RECORD_OFF : BTN_RECORD_ON);

    if (this->isRecordOn)
    {
        this->recordFrames.clear();

        this->recordStartTime = QTime::currentTime();
        this->timerVideoRecord->start(1000); // Update every second
    }
    else
    {
        if (this->recordFrames.size() > 0)
        {
            MainWindow::RecordVideo(this->recordFrames, this->recordDir, this->recordFormat, this->recordFrameRate, this->recordQuality);
            this->recordFrames.clear();
        }
    }
}

void MainWindow::btnRecordOption_Click()
{
    dialog_record_option dialog(this->recordDir, this->recordFormat, this->recordFrameRate, this->recordQuality, this->recordTimeLimit, this);

    if (dialog.exec() == QDialog::Accepted)
    {
        this->recordDir = dialog.getVideoDirectory();
        this->recordFormat = dialog.getVideoFormat();
        this->recordFrameRate = dialog.getFrameRate();
        this->recordQuality = dialog.getQuality();
        this->recordTimeLimit = dialog.getTimeLimit();
    }
}


void MainWindow::sliderExposureTime_sliderMoved(int position)
{
    // trackbar가 정수이므로 0.1을 곱한다.
    double value = ui->sliderExposureTime->value() * 0.1;

    // time이 microsecond이고 range가 0.1-50000이기 때문에 0.01을 곱한다.
    Miicam_put_ExpoTime(this->miiHcam, (unsigned int)(value * 100.0));

    // label도 업데이트
    ui->editExposureTime->setPlainText(QString::number(value, 'f', 1));
}

void MainWindow::editExposureTime_editingFinished()
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
            QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            value = roundToDecimalPlaces(ui->sliderExposureTime->value() * 0.1, 1);
            ui->editExposureTime->setPlainText(QString::number(value, 'f', 1));
        }
    }
    else
    {
        QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        value = roundToDecimalPlaces(ui->sliderExposureTime->value() * 0.1, 1);
        ui->editExposureTime->setPlainText(QString::number(value, 'f', 1));
    }
}

void MainWindow::sliderGain_sliderMoved(int position)
{
    int value = ui->sliderGain->value();

    Miicam_put_ExpoAGain(this->miiHcam, (unsigned short)(value));

    // label도 업데이트
    ui->editGain->setPlainText(QString::number(value));
}

void MainWindow::editGain_editingFinished()
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
            QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            ui->editGain->setPlainText(QString::number(ui->sliderGain->value()));
        }
    }
    else
    {
        QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        ui->editGain->setPlainText(QString::number(ui->sliderGain->value()));
    }
}

void MainWindow::sliderContrast_sliderMoved(int position)
{
    int value = ui->sliderContrast->value();

    Miicam_put_Contrast(this->miiHcam, value);

    // label도 업데이트
    ui->editContrast->setPlainText(QString::number(value));
}

void MainWindow::editContrast_editingFinished()
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
            QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            ui->editContrast->setPlainText(QString::number(ui->sliderContrast->value()));
        }
    }
    else
    {
        QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        ui->editContrast->setPlainText(QString::number(ui->sliderContrast->value()));
    }
}

void MainWindow::sliderGamma_sliderMoved(int position)
{
    int value = ui->sliderGamma->value();

    Miicam_put_Gamma(this->miiHcam, value);

    // label도 업데이트
    ui->editGamma->setPlainText(QString::number(value));
}

void MainWindow::editGamma_editingFinished()
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
            QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            ui->editGamma->setPlainText(QString::number(ui->sliderGamma->value()));
        }
    }
    else
    {
        QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        ui->editGamma->setPlainText(QString::number(ui->sliderGamma->value()));
    }
}

void MainWindow::btnCurveSetting_Click()
{
    dialog_contrast_curve dialog(this->presetsContrastCurve, ui->cbCurvePreset->currentIndex() - 1, this->isUpdateContrastCurve, this);

    if (dialog.exec() == QDialog::Accepted)
    {
        this->presetsContrastCurve = dialog.getPresets();
        this->isUpdateContrastCurve = dialog.getEnable();
        int index = this->isUpdateContrastCurve ? dialog.getSelectedIndex() + 1 : 0;

        QJsonArray jsonArray;
        convertPresetsImageCurveToJsonArray(this->presetsContrastCurve, jsonArray);

        QString pathPreset = QCoreApplication::applicationDirPath() + PATH_JSON_CONTRAST_CURVE;
        saveJsonFile(pathPreset, jsonArray);

        MainWindow::UpdatePresetContrastCurve(this->presetsContrastCurve, index);
    }
}

void MainWindow::cbCurvePreset_SelectedIndexChanged(int index)
{
    // 0이면 none
    if (index > 0)
    {
        preset_contrast_curve preset = this->presetsContrastCurve[index - 1];

        QMutexLocker locker(&contrastCurvesMutex);

        if (this->contrastCurves)
        {
            g_object_unref(this->contrastCurves);
            this->contrastCurves = nullptr;
        }

        this->contrastCurves = gegl_curve_new(0.0, 1.0);

        for (const curve_point& point : preset.GetPoints())
        {
            gegl_curve_add_point(this->contrastCurves, point.GetX(), point.GetY());
        }

        this->isUpdateContrastCurve = true;
    }
    else
    {
        this->isUpdateContrastCurve = false;
    }
}

void MainWindow::chkDarkFieldCorrection_CheckedChanged(Qt::CheckState checkState)
{
    int option = ui->chkDarkFieldCorrection->isChecked() ? 1 : 0;
    Miicam_put_Option(this->miiHcam, MIICAM_OPTION_DFC, option);
}

void MainWindow::btnDarkFieldCorrection_Click()
{
    bool ok;
    int value = ui->editDarkFieldCorrectionQuantity->toPlainText().toInt(&ok);

    if (ok)
    {
        if (value >= MIICAM_DARK_FIELD_QUANTITY_MIN && value <= MIICAM_DARK_FIELD_QUANTITY_MAX)
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
            QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_RANGE);
            ui->editGamma->setPlainText(QString::number(MIICAM_DARK_FIELD_QUANTITY_DEFAULT));
        }
    }
    else
    {
        QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_VALUE);
        ui->editGamma->setPlainText(QString::number(MIICAM_DARK_FIELD_QUANTITY_DEFAULT));
    }
}

void MainWindow::btnGroupCooling_Click(int id)
{
    bool enable = id == 1;
    ui->sliderTemperature->setEnabled(enable);
    ui->editTemperature->setEnabled(enable);
}

void MainWindow::sliderTemperature_sliderMoved(int position)
{
    // trackbar가 정수이므로 0.1을 곱한다.
    double value = ui->sliderTemperature->value() * 0.1;

    // temperature는 3.2도를 32로 받기 때문에 10을 곱한다.
    Miicam_put_Temperature(this->miiHcam, (short)(value * 10.0));

    // label도 업데이트
    ui->editTemperature->setPlainText(QString::number(value, 'f', 1));
}

void MainWindow::editTemperature_editingFinished()
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
            QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            value = roundToDecimalPlaces(ui->sliderTemperature->value() * 0.1, 1);
            ui->editTemperature->setPlainText(QString::number(value, 'f', 1));
        }
    }
    else
    {
        QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        value = roundToDecimalPlaces(ui->sliderTemperature->value() * 0.1, 1);
        ui->editTemperature->setPlainText(QString::number(value, 'f', 1));
    }
}

void MainWindow::btnZoomIn_Click()
{
    this->zoomFactor += ZOOM_VALUE;

    if (this->zoomFactor > ZOOM_MAX)
    {
        this->zoomFactor = ZOOM_MAX;
    }

    // 현재 창 크기 기준으로 zoom 조절. 실제 이미지 크기로 하면 창 크기를 넘어선다.
    ui->gvPreview->fitInView();
    ui->gvPreview->scale(this->zoomFactor, this->zoomFactor);

    ui->gvVideo->fitInView();
    ui->gvVideo->scale(this->zoomFactor, this->zoomFactor);

    ui->lbZoom->setText(QString("Zoom x%1").arg(this->zoomFactor, 0, 'f', 2));

}

void MainWindow::btnZoomOut_Click()
{
    this->zoomFactor -= ZOOM_VALUE;

    if (this->zoomFactor < ZOOM_MIN)
    {
        this->zoomFactor = ZOOM_MIN;
    }

    // 현재 창 크기 기준으로 zoom 조절. 실제 이미지 크기로 하면 창 크기를 넘어선다.
    ui->gvPreview->fitInView();
    ui->gvPreview->scale(this->zoomFactor, this->zoomFactor);

    ui->gvVideo->fitInView();
    ui->gvVideo->scale(this->zoomFactor, this->zoomFactor);

    ui->lbZoom->setText(QString("Zoom x%1").arg(this->zoomFactor, 0, 'f', 2));
}

void MainWindow::btnBrightnessContrast_Click()
{
    dialog_brightness_contrast dialog(this->presetsBrightnessContrast, this->gegl_brightness, this->gegl_contrast, this->isUpdateBrightnessContrast, this);

    if (dialog.exec() == QDialog::Accepted)
    {
        this->presetsBrightnessContrast = dialog.getPresets();
        this->gegl_brightness = dialog.getBrightness();
        this->gegl_contrast = dialog.getContrast();
        this->isUpdateBrightnessContrast = dialog.getEnable();

        QJsonArray jsonArray;
        convertBrightnessContrastPresetsToJsonArray(this->presetsBrightnessContrast, jsonArray);

        QString pathPreset = QCoreApplication::applicationDirPath() + PATH_JSON_BRIGHTNESS_CONTRAST;
        saveJsonFile(pathPreset, jsonArray);
    }
}

void MainWindow::btnStress_Click()
{
    dialog_stress dialog(this->presetsStress, this->gegl_stress_radius, this->gegl_stress_samples, this->gegl_stress_iterations, this->gegl_stress_enhance_shadows, this->isUpdateStress, this);

    if (dialog.exec() == QDialog::Accepted)
    {
        this->presetsStress = dialog.getPresets();
        this->gegl_stress_radius = dialog.getRadius();
        this->gegl_stress_samples = dialog.getSamples();
        this->gegl_stress_iterations = dialog.getIterations();
        this->gegl_stress_enhance_shadows = dialog.getEnhanceShadows();
        this->isUpdateStress = dialog.getEnable();

        QJsonArray jsonArray;
        convertStressPrestesToJsonArray(this->presetsStress, jsonArray);

        QString pathPreset = QCoreApplication::applicationDirPath() + PATH_JSON_STRESS;
        saveJsonFile(pathPreset, jsonArray);
    }
}

void MainWindow::btnStretchContrast_Click()
{
    dialog_stretch_contrast dialog(this->gegl_stretch_contrast_keep_colors, this->gegl_stretch_contrast_perceptual, this->isUpdateStretchContrast);

    if (dialog.exec() == QDialog::Accepted)
    {
        this->gegl_stretch_contrast_keep_colors = dialog.getKeepColors();
        this->gegl_stretch_contrast_perceptual = dialog.getNonLinearComponents();
        this->isUpdateStretchContrast = dialog.getEnable();
    }
}

/////////////////////// video
void MainWindow::btnLoadVideo_Click()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty())
    {
        ui->lvVideo->setRootIndex(this->filesystemVideo->setRootPath(dir));
        ui->lbDirVideo->setText(dir);
    }
}

void MainWindow::lvVideo_Click(const QModelIndex &index)
{
    // 일단 update를 중지시킨다.
    this->isVideoPlay = false;

    QString filePath = this->filesystemVideo->filePath(index);

    progressDialog->reset();
    progressDialog->show();

    VideoLoader *loader = new VideoLoader(filePath);
    connect(loader, &VideoLoader::progress, this, &MainWindow::onVideoLoadingProgress);
    connect(loader, &VideoLoader::finished, this, &MainWindow::onVideoLoadingFinished);
    connect(loader, &VideoLoader::finished, loader, &QObject::deleteLater);

    loader->start();
}

void MainWindow::onVideoLoadingProgress(int value)
{
    progressDialog->setValue(value);
}

void MainWindow::onVideoLoadingFinished(bool success, const std::vector<QImage>& frames, double frameRate, int totalFrames)
{
    progressDialog->hide(); // Hide the progress dialog

    if (success)
    {
        this->videoFrames = frames;
        this->videoFrameRates = frameRate;
        this->videoTotalFrame = totalFrames;
        this->currentFrame = 0;

        ui->sliderVideo->setRange(0, this->videoTotalFrame - 1);
        ui->sliderVideo->setSingleStep(1);
        ui->sliderVideo->setPageStep(10);
        ui->sliderVideo->setValue(0);

        ui->lbVideoFrame->setText(QString("%1 / %2").arg(ui->sliderVideo->value()).arg(this->videoTotalFrame));

        ui->btnPlayVideo->setEnabled(true);
        ui->btnStopVideo->setEnabled(true);
        ui->sliderVideo->setEnabled(true);

        ui->btnPlayVideo->setText("Pause");

        this->isVideoPlay = true;
    }
    else
    {
        QMessageBox::warning(this, "Error", "Could not open the video file.");
    }
}

void MainWindow::btnPlayVideo_Click()
{
    this->isVideoPlay = !this->isVideoPlay;
    ui->btnPlayVideo->setText(!this->isVideoPlay ? BTN_PLAY : BTN_PAUSE);
}

void MainWindow::btnStopVideo_Click()
{
    this->isVideoPlay = false;
    this->currentFrame = 0;

    // play를 중지시켰으므로 직접 업데이트 한다.
    this->resultVideo = this->videoFrames[this->currentFrame];
    UpdateVideoUI();
}

void MainWindow::sliderVideo_sliderMoved(int position)
{
    this->currentFrame = position;

    // play 중이 아니면 직접 업데이트한다.
    if (!this->isVideoPlay)
    {
        this->resultVideo = this->videoFrames[this->currentFrame];
        UpdateVideoUI();
    }
}

/////////////////////// frame
void MainWindow::btnLoadFrame_Click()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty())
    {
        ui->lvFrame->setRootIndex(this->filesystemFrame->setRootPath(dir));
        ui->lbDirFrames->setText(dir);
    }
}

void MainWindow::lvFrame_Click(const QModelIndex &index)
{
    QString filePath = this->filesystemFrame->filePath(index);
    QImage image(filePath);
    ui->gvFrame->setImage(image);
    ui->gvFrame->fitInView();
}


////////////////////////////////////////
// non-ui
////////////////////////////////////////

void MainWindow::UpdatePreview()
{
    while (this->isOn)
    {
        if (this->isCameraPlay && this->rawCameraData)
        {
            QImage source = QImage(this->rawCameraData, this->rawCameraWidth, this->rawCameraHeight, this->imageFormat);

            // gegl에서는 rgba를 받기 때문에 무조건 rgba로 바꿔야 한다.
            QImage formattedSource = source.convertToFormat(QImage::Format_RGBA8888);
            //QImage formattedSource = this->imageFormat != QImage::Format_RGBA8888 ? source.convertToFormat(QImage::Format_RGBA8888) : source;

            UpdateGeglContrast(
                /*source*/ formattedSource
                , /*brightness*/ this->gegl_brightness
                , /*contrast*/ this->gegl_contrast
                , /*radius*/ this->gegl_stress_radius
                , /*samples*/ this->gegl_stress_samples
                , /*iterations*/ this->gegl_stress_iterations
                , /*enhanceShadows*/ this->gegl_stress_enhance_shadows
                , /*keepColors*/ this->gegl_stretch_contrast_keep_colors
                , /*perceptual*/ this->gegl_stretch_contrast_perceptual
                , /*curve*/ this->contrastCurves
                , /*samplingPoints*/ this->gegl_contrast_curve_sampling_points
                , /*isUpdateBrightnessContrast*/ this->isUpdateBrightnessContrast
                , /*isUpdateStress*/ this->isUpdateStress
                , /*isUpdateStretchContrast*/ this->isUpdateStretchContrast
                , /*isUpdateContrastCurve*/ this->isUpdateContrastCurve
            );

            // gegl을 적용한 후에 result에 넣는다. 그래야 video나 capture에서 gegl이 적용된 이미지가 사용될 수 있음.
            this->resultPreview = formattedSource.convertToFormat(QImage::Format_RGB888);

            if (this->isRecordOn)
            {
                cv::Mat mat(this->resultPreview.height(), this->resultPreview.width(), CV_8UC3, const_cast<uchar*>(this->resultPreview.bits()), this->resultPreview.bytesPerLine());
                cv::Mat matBGR;
                cv::cvtColor(mat, matBGR, cv::COLOR_RGB2BGR);  // rgb -> bgr

                this->recordFrames.emplace_back(matBGR);
            }

            QMetaObject::invokeMethod(this, "UpdatePreviewUI", Qt::QueuedConnection);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_CAMERA)); // Frame rate delay
    }
}

void MainWindow::UpdateVideo()
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

        std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_VIDEO)); // Frame rate delay
    }
}

void MainWindow::UpdatePreviewUI()
{
    ui->gvPreview->setImage(this->resultPreview);

    if (std::abs(this->zoomFactor - 1.0f) <= std::numeric_limits<float>::epsilon())
    {
        ui->gvPreview->fitInView();
    }

    if (this->isRecordOn)
    {
        QTime currentTime = QTime::currentTime();
        int elapsedSeconds = this->recordStartTime.secsTo(currentTime);
        QTime elapsedTime(0, 0);
        elapsedTime = elapsedTime.addSecs(elapsedSeconds);

        ui->lbRunTime->setText("Recoding Time: " + elapsedTime.toString("hh:mm:ss"));

        if (this->recordTimeLimit > 0 && elapsedTime.second() >= this->recordTimeLimit)
        {
            ui->btnRecordOn->setText(BTN_RECORD_OFF);
        }
    }
}

void MainWindow::UpdateVideoUI()
{
    ui->gvVideo->setImage(this->resultVideo);

    if (std::abs(zoomFactor - 1.0f) <= std::numeric_limits<float>::epsilon())
    {
        ui->gvVideo->fitInView();
    }

    ui->lbVideoFrame->setText(QString("%1 / %2").arg(this->currentFrame + 1).arg(this->videoTotalFrame));
    ui->sliderVideo->setValue(this->currentFrame);

    if (!this->isVideoPlay)
    {
        ui->btnPlayVideo->setText(BTN_PLAY);
    }
}

void MainWindow::LoadPresets()
{
    QJsonArray jsonArray;

    if (loadJsonFile(QCoreApplication::applicationDirPath() + PATH_JSON_BRIGHTNESS_CONTRAST, jsonArray))
    {
        this->presetsBrightnessContrast = convertJsonToBrightnessContrastPresets(jsonArray);
    }

    if (loadJsonFile(QCoreApplication::applicationDirPath() + PATH_JSON_STRESS, jsonArray))
    {
        this->presetsStress = convertJsonToStressPrestes(jsonArray);
    }

    if (loadJsonFile(QCoreApplication::applicationDirPath() + PATH_JSON_CONTRAST_CURVE, jsonArray))
    {
        this->presetsContrastCurve = convertJsonToPresetsImageCurve(jsonArray);
    }
}

void MainWindow::UpdatePresetContrastCurve(const std::vector<preset_contrast_curve>& presets, const int index)
{
    ui->cbCurvePreset->clear();

    ui->cbCurvePreset->addItem(KEY_NONE);

    if (presets.size() > 0)
    {
        for (const preset_contrast_curve& preset : presets)
        {
            QString message = QString("index: %1, points: %2")
                                  .arg(preset.GetIndex()) // 'f' for floating point, 2 decimal places
                                  .arg(preset.GetPoints().size());
            ui->cbCurvePreset->addItem(message);
        }
    }

    ui->cbCurvePreset->setCurrentIndex(index);
}

void MainWindow::InitGegl()
{
    // 실행파일 경로 아래 lib에 babl, gegl의 plugin dll을 복사해 놨기 때문에 프로그램 실행 중에 사용할 환경 변수를 등록해 둠. 프로그램 종료시 해제됨.
    QString appDir = QCoreApplication::applicationDirPath();
    qputenv("BABL_PATH", (appDir + "/lib/babl-0.1").toUtf8());
    qputenv("GEGL_PATH", (appDir + "/lib/gegl-0.4").toUtf8());

    // gegl 초기화
    gegl_init(nullptr, nullptr);
}

void MainWindow::CloseGegl()
{
    if (this->contrastCurves)
    {
        g_object_unref(this->contrastCurves);
    }

    // gegl 종료
    gegl_exit();
}

void MainWindow::FindCamera()
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

            MainWindow::OpenCamera();
        }
        else
        {
            QMessageBox::warning(this, "Warning", "No camera found.");
        }
    }
}

void MainWindow::OpenCamera()
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
            MainWindow::InitCameraResolution();

            // open에 성공하면 true
            ui->cbResolution->setEnabled(true);
            ui->cbFormat->setEnabled(true);
            ui->btnPlayCamera->setEnabled(true);

            ui->btnPlayCamera->setText(BTN_PLAY);
        }

    }
}

void MainWindow::CloseCamera()
{
    ui->btnPlayCamera->setEnabled(false);
    ui->btnPlayCamera->setText(BTN_PLAY);

    if (this->miiHcam)
    {
        Miicam_Close(this->miiHcam);
        this->miiHcam = nullptr;
    }

    if (this->rawCameraData)
    {
        delete[] this->rawCameraData;
    }
}

void MainWindow::StartCamera()
{
    if (this->rawCameraData)
    {
        delete[] this->rawCameraData;
        this->rawCameraData = nullptr;
    }

    this->rawCameraData = new uchar[TDIBWIDTHBYTES(imageWidth * 24) * this->imageHeight];

    MainWindow::UpdateExposureTime();
    MainWindow::UpdateSensorTemperature();

    if ((this->miiDevice.model->flag & MIICAM_FLAG_MONO) == 0)
    {
        handleTempTintEvent();
    }

    handleExpoEvent();

    if (SUCCEEDED(Miicam_StartPullModeWithCallback(this->miiHcam, eventCallBack, this)))
    {
        this->isCameraRun = true;

        // fps update
        timerFPS->start(1000);
    }
    else
    {
        CloseCamera();
        QMessageBox::warning(this, "Warning", "Failed to start camera.");
    }
}

void MainWindow::InitCameraResolution()
{
    ui->cbResolution->clear();
    for (unsigned i = 0; i < this->miiDevice.model->preview; ++i)
    {
        ui->cbResolution->addItem(QString::asprintf("%u x %u", this->miiDevice.model->res[i].width, this->miiDevice.model->res[i].height));
    }
    ui->cbResolution->setCurrentIndex(this->resolutionIndex);
}

void MainWindow::UpdateExposureTime()
{
    unsigned int nMin, nMax, nDef, nTime;
    Miicam_get_ExpTimeRange(this->miiHcam, &nMin, &nMax, &nDef);
    Miicam_get_ExpoTime(this->miiHcam, &nTime);

    double min = nMin * 0.01;
    double max = nMax * 0.01;
    double def = nDef * 0.01;
    double value = nTime * 0.01;

    if (min < MIICAM_EXPOSURE_TIME_MIN)
    {
        min = MIICAM_EXPOSURE_TIME_MIN;
    }

    if (max > MIICAM_EXPOSURE_TIME_MAX)
    {
        max = MIICAM_EXPOSURE_TIME_MAX;
    }

    ui->sliderExposureTime->setMinimum((int)(min * 10.0));
    ui->sliderExposureTime->setMaximum((int)(max * 10.0));
    ui->sliderExposureTime->setValue((int)(value * 10.0));
    ui->editExposureTime->setPlainText(QString::number(value, 'f', 1));
}

void MainWindow::UpdateSensorTemperature()
{
    // 강제로 -50도로 초기화
    double value = MIICAM_TEMPERATURE_MIN * 10.0;
    Miicam_put_Temperature(this->miiHcam, (short)value);

    //short temperature;
    //Miicam_get_Temperature(this->miiHcam, &temperature);

    //// temperature는 3.2도를 32로 받기 때문에 0.1을 곱한다.
    //double value = temperature * 0.1;

    ui->sliderTemperature->setMinimum((int)(MIICAM_TEMPERATURE_MIN * 10.0));
    ui->sliderTemperature->setMaximum((int)(MIICAM_TEMPERATURE_MAX * 10.0));
    ui->sliderTemperature->setValue((int)value);
    ui->editTemperature->setPlainText(QString::number(value * 0.1, 'f', 1));
}

void MainWindow::onTimerFpsCallback()
{
    unsigned nFrame = 0, nTime = 0, nTotalFrame = 0;

    if (miiHcam && SUCCEEDED(Miicam_get_FrameRate(this->miiHcam, &nFrame, &nTime, &nTotalFrame)) && (nTime > 0))
    {
        ui->lbFPS->setText(QString::asprintf("%u, fps = %.1f", nTotalFrame, nFrame * 1000.0 / nTime));
    }
}

void MainWindow::eventCallBack(unsigned nEvent, void* pCallbackCtx)
{
    MainWindow* pThis = reinterpret_cast<MainWindow*>(pCallbackCtx);
    emit pThis->evtCallback(nEvent);
}

void MainWindow::onMiiCameraCallback(unsigned nEvent)
{
    /* this run in the UI thread */
    if (this->miiHcam)
    {
        if (MIICAM_EVENT_IMAGE == nEvent)
        {
            handleImageEvent();
        }
        else if (MIICAM_EVENT_EXPOSURE == nEvent)
        {
            handleExpoEvent();
        }
        else if (MIICAM_EVENT_TEMPTINT == nEvent)
        {
            handleTempTintEvent();
        }
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

void MainWindow::handleImageEvent()
{
    QMutexLocker locker(&imageMutex);

    if (SUCCEEDED(Miicam_PullImage(this->miiHcam, this->rawCameraData, 24, &this->rawCameraWidth, &this->rawCameraHeight)))
    {
    }
}

void MainWindow::handleExpoEvent()
{
    unsigned time = 0;
    unsigned short gain = 0;
    Miicam_get_ExpoTime(this->miiHcam, &time);
    Miicam_get_ExpoAGain(this->miiHcam, &gain);

    // {
    //     const QSignalBlocker blocker(m_slider_expoTime);
    //     m_slider_expoTime->setValue(int(time));
    // }
    // {
    //     const QSignalBlocker blocker(m_slider_expoGain);
    //     m_slider_expoGain->setValue(int(gain));
    // }
    // m_lbl_expoTime->setText(QString::number(time));
    // m_lbl_expoGain->setText(QString::number(gain));
}

void MainWindow::handleTempTintEvent()
{
    int nTemp = 0, nTint = 0;
    if (SUCCEEDED(Miicam_get_TempTint(this->miiHcam, &nTemp, &nTint)))
    {
        // {
        //     const QSignalBlocker blocker(m_slider_temp);
        //     m_slider_temp->setValue(nTemp);
        // }
        // {
        //     const QSignalBlocker blocker(m_slider_tint);
        //     m_slider_tint->setValue(nTint);
        // }
        // m_lbl_temp->setText(QString::number(nTemp));
        // m_lbl_tint->setText(QString::number(nTint));
    }
}

// void MainWindow::handleStillImageEvent()
// {
//     unsigned width = 0, height = 0;
//     if (SUCCEEDED(Miicam_PullStillImage(this->miiHcam, nullptr, 24, &width, &height))) // peek
//     {
//         std::vector<uchar> vec(TDIBWIDTHBYTES(width * 24) * height);
//         if (SUCCEEDED(Miicam_PullStillImage(this->miiHcam, &vec[0], 24, &width, &height)))
//         {
//             QImage image(&vec[0], width, height, QImage::Format_RGB888);

//             // 현재 날짜와 시간을 가져오기
//             QDateTime currentDateTime = QDateTime::currentDateTime();
//             QString formattedDateTime = currentDateTime.toString("yyyy_MM_dd_hh_mm_ss");

//             QString filename = captureDir + QString("/%1.jpg").arg(formattedDateTime);

//              // 디렉토리가 존재하지 않으면 생성
//             QDir dir(captureDir);
//             if (!dir.exists()) {
//                 dir.mkpath(captureDir);
//             }
//             image.save(filename);
//         }
//     }
// }


// void MainWindow::SetVideoPlay(bool value)
// {
//     this->isVideoPlay = true;

//     const QSignalBlocker blocker(ui->btnPlayVideo);
//     ui->btnPlayVideo->setText(!value ? BTN_PLAY : MENU_PAUSE);
// }


bool MainWindow::RecordVideo(
    std::vector<cv::Mat>& frames
    , const QString& recordDir
    , const VideoFormatType format
    , const double frameRate
    , const int quality
)
{
    if (frames.size() > 0)
    {
        QDir dir(recordDir);
        if (!dir.exists())
        {
            dir.mkpath(recordDir);
        }

        QString timestamp = QDateTime::currentDateTime().toString(FORMAT_DATE_TIME);
        QString filePath = dir.absoluteFilePath(timestamp + getVideoExtension(format));
        int fourcc = getVideoFourcc(format);

        try
        {
            // Create VideoWriter object
            cv::VideoWriter writer(filePath.toStdString(), fourcc, frameRate, cv::Size(frames[0].cols, frames[0].rows));

            // quality는 특정 format에만 적용된다.
            if (format == VideoFormatType::MJPEG)
            {
                writer.set(cv::VIDEOWRITER_PROP_QUALITY, quality);
            }

            // Write frames to video file
            for (const cv::Mat& mat : frames)
            {
                writer.write(mat);
            }

            // Release the VideoWriter
            writer.release();

            return true;
        }
        catch (cv::Exception ex)
        {
            QMessageBox::critical(this, TITLE_ERROR, QString::fromStdString(ex.msg));
        }
    }

    return false;
}


void MainWindow::UpdateGeglContrast(
    QImage& source,
    const double brightness,
    const double contrast,
    const int radius,
    const int samples,
    const int iterations,
    const bool enhanceShadows,
    const bool keepColors,
    const bool perceptual,
    const GeglCurve* curve,
    const int samplingPoints,
    const bool isUpdateBrightnessContrast,
    const bool isUpdateStress,
    const bool isUpdateStretchContrast,
    const bool isUpdateContrastCurve
    )
{
    // 하나라도 true여야 수행한다.
    if (isUpdateBrightnessContrast || isUpdateStress || isUpdateStretchContrast || isUpdateContrastCurve)
    {
        // Create GEGL buffers
        GeglBuffer* input_buffer = gegl_buffer_new(GEGL_RECTANGLE(0, 0, source.width(), source.height()), babl_format("R'G'B'A u8"));
        GeglBuffer* output_buffer = gegl_buffer_new(GEGL_RECTANGLE(0, 0, source.width(), source.height()), babl_format("R'G'B'A u8"));

        // Set input buffer data
        gegl_buffer_set(input_buffer, nullptr, 0, babl_format("R'G'B'A u8"), source.bits(), GEGL_AUTO_ROWSTRIDE);

        // Create GEGL graph
        GeglNode* graph = gegl_node_new();
        GeglNode* input = gegl_node_new_child(graph, "operation", "gegl:buffer-source", "buffer", input_buffer, nullptr);
        GeglNode* output = gegl_node_new_child(graph, "operation", "gegl:write-buffer", "buffer", output_buffer, nullptr);

        GeglNode* brightness_contrast = nullptr;
        GeglNode* stress = nullptr;
        GeglNode* stretch_contrast = nullptr;
        GeglNode* contrast_curve = nullptr;

        // gegl_node_set()은 gegl_node_link()를 하기 전에 마쳐야 한다.
        if (isUpdateBrightnessContrast)
        {
            brightness_contrast = gegl_node_new_child(graph, "operation", "gegl:brightness-contrast", nullptr);
            gegl_node_set(brightness_contrast, "brightness", brightness, "contrast", contrast, nullptr);
        }

        if (isUpdateStress)
        {
            stress = gegl_node_new_child(graph, "operation", "gegl:stress", nullptr);
            gegl_node_set(stress, "radius", radius, "samples", samples, "iterations", iterations, "enhance-shadows", enhanceShadows, nullptr);
        }

        if (isUpdateStretchContrast)
        {
            stretch_contrast = gegl_node_new_child(graph, "operation", "gegl:stretch-contrast", nullptr);
            gegl_node_set(stretch_contrast, "keep-colors", keepColors, "perceptual", perceptual, nullptr);
        }

        if (isUpdateContrastCurve && contrastCurves)
        {
            contrast_curve = gegl_node_new_child(graph, "operation", "gegl:contrast-curve", nullptr);
            gegl_node_set(contrast_curve, "curve", contrastCurves, "sampling-points", samplingPoints, nullptr);
        }

        // Link nodes
        GeglNode* last_node = input;
        if (brightness_contrast)
        {
            gegl_node_link(last_node, brightness_contrast);
            last_node = brightness_contrast;
        }
        if (stress)
        {
            gegl_node_link(last_node, stress);
            last_node = stress;
        }
        if (stretch_contrast)
        {
            gegl_node_link(last_node, stretch_contrast);
            last_node = stretch_contrast;
        }
        if (contrast_curve)
        {
            gegl_node_link(last_node, contrast_curve);
            last_node = contrast_curve;
        }
        gegl_node_link(last_node, output);

        gegl_node_process(output);

        // Get output buffer data
        gegl_buffer_get(output_buffer, nullptr, 1.0, babl_format("R'G'B'A u8"), source.bits(), GEGL_AUTO_ROWSTRIDE, GEGL_ABYSS_NONE);

        // Unreference GEGL buffers and graph
        g_object_unref(input_buffer);
        g_object_unref(output_buffer);
        g_object_unref(graph);
    }
}





