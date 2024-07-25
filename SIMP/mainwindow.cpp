#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pixel_format_type.h"
#include "utils.h"
#include "dialog_brightness_contrast.h"
#include "dialog_stretch_contrast.h"
#include "dialog_stress.h"

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
    , modelFrames(new QFileSystemModel(this))
    , mpVideoFile(new QMediaPlayer(this))
    , timerFPS(new QTimer(this))
    , timerVideoRecord(new QTimer(this))
    , btnGroupCooling(new QButtonGroup(this))
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

    MainWindow::ConnectUI();
    MainWindow::InitUI();

    // 일단 false로 시작
    MainWindow::EnablePreviewUI(false);
    MainWindow::EnableDarkfieldUI(false);
    MainWindow::EnableCoolingUI(false);

    // thread 시작
    this->isOn = true;
    updateThread = std::thread(&MainWindow::UpdatePreview, this);

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

    if (updateThread.joinable()) {
        updateThread.join();
    }

    MainWindow::CloseGegl();
    MainWindow::CloseCamera();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (ui->tabWidget->currentWidget() == ui->tabCapture)
    {
        ui->gvFrameCapture->fitInView();
    }
}


////////////////////////////////////////
// UI
////////////////////////////////////////
void MainWindow::ConnectUI()
{
    connect(this, &MainWindow::evtCallback, this, &MainWindow::onMiiCameraCallback);
    connect(timerFPS, &QTimer::timeout, this, &MainWindow::onTimerFpsCallback);

    // preview
    connect(ui->gvPreview, &CustomGraphicsView::mousePositionChanged, this, &MainWindow::UpdatePreviewMousePosition);
    connect(ui->gvFrameCapture, &CustomGraphicsView::mousePositionChanged, this, &MainWindow::UpdateFrameMousePosition);

    connect(ui->cbResolution, &QComboBox::currentIndexChanged, this, &MainWindow::cbResoution_SelectedIndexChanged);
    connect(ui->cbFormat, &QComboBox::currentIndexChanged, this, &MainWindow::cbFormat_SelectedIndexChanged);

    connect(ui->btnPlayCamera, &QPushButton::clicked, this, &MainWindow::btnPlayCamera_Click);
    connect(ui->btnStopCamera, &QPushButton::clicked, this, &MainWindow::btnStopCamera_Click);
    connect(ui->btnCaptureCamera, &QPushButton::clicked, this, &MainWindow::btnCaptureCamera_Click);

    connect(ui->chkRecord, &QCheckBox::checkStateChanged, this, &MainWindow::chkRecord_CheckedChanged);
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

    connect(ui->chkDarkfield, &QCheckBox::checkStateChanged, this, &MainWindow::chkDarkfield_CheckedChanged);
    connect(ui->editDarkfieldQuantity, &CustomPlainTextEdit::editingFinished, this, &MainWindow::editDarkfieldQuantity_editingFinished);
    connect(ui->btnDarkfieldCapture, &QPushButton::clicked, this, &MainWindow::btnDarkfieldCapture_Click);

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


    // video
    connect(ui->btnLoadVideo, &QPushButton::clicked, this, &MainWindow::btnLoadVideo_Click);
    connect(ui->btnPlayVideo, &QPushButton::clicked, this, &MainWindow::btnPlayVideo_Click);
    connect(ui->btnStopVideo, &QPushButton::clicked, this, &MainWindow::btnStopVideo_Click);

    connect(mpVideoFile, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onVideoStatusChanged);

    // capture
    connect(ui->btnLoadFrame, &QPushButton::clicked, this, &MainWindow::btnLoadFrame_Click);
    connect(ui->lvFrames, &QListView::clicked, this, &MainWindow::lvFrames_Click);
}

void MainWindow::InitUI()
{
    // update combobox - 나중에 옮길 것.
    ui->cbFormat->clear();
    for (const auto& format : {PixelFormatType::RGB24, PixelFormatType::RGB32, PixelFormatType::Raw})
    {
        ui->cbFormat->addItem(toString(format));
    }
    ui->cbFormat->setCurrentIndex(0);

    // init gain, contrast, gamma
    int MIICAM_EXPOGAIN_MAX = 5000;
    ui->sliderGain->setMinimum(MIICAM_EXPOGAIN_MIN);
    ui->sliderGain->setMaximum(MIICAM_EXPOGAIN_MAX);
    ui->sliderGain->setValue(MIICAM_EXPOGAIN_DEF);
    ui->editGain->setPlainText(QString::number(round(MIICAM_EXPOGAIN_DEF)));

    ui->sliderContrast->setMinimum(MIICAM_CONTRAST_MIN);
    ui->sliderContrast->setMaximum(MIICAM_CONTRAST_MAX);
    ui->sliderContrast->setValue(MIICAM_CONTRAST_DEF);
    ui->editContrast->setPlainText(QString::number(round(MIICAM_CONTRAST_DEF)));

    ui->sliderGamma->setMinimum(MIICAM_GAMMA_MIN);
    ui->sliderGamma->setMaximum(MIICAM_GAMMA_MAX);
    ui->sliderGamma->setValue(MIICAM_GAMMA_DEF);
    ui->editGamma->setPlainText(QString::number(round(MIICAM_GAMMA_DEF)));

    mpVideoFile->setVideoOutput(ui->videoFile);

    // tab frame
    // Check if the captures directory exists, and create it if it doesn't
    QDir dir(captureDir);
    if (!dir.exists()) {
        dir.mkpath(captureDir);
    }

    // Set model properties
    this->modelFrames->setRootPath(captureDir);
    this->modelFrames->setNameFilters(QStringList() << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp" << "*.gif");
    this->modelFrames->setNameFilterDisables(false);

    ui->lvFrames->setModel(modelFrames);
    ui->lvFrames->setRootIndex(modelFrames->index(captureDir)); // Set the root index
    ui->lbDirFrames->setText(captureDir);

    ui->btnPlayVideo->setEnabled(false);
    ui->btnStopVideo->setEnabled(false);
}

void MainWindow::EnablePreviewUI(bool isPlay)
{
    ui->cbResolution->setEnabled(!isPlay);
    ui->cbFormat->setEnabled(!isPlay);

    ui->btnStopCamera->setEnabled(isPlay);
    ui->btnCaptureCamera->setEnabled(isPlay);

    ui->chkRecord->setEnabled(isPlay);
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

    ui->chkDarkfield->setEnabled(isPlay);

    ui->rbCoolingOn->setEnabled(isPlay);
    ui->rbCoolingOff->setEnabled(isPlay);

    ui->btnZoomIn->setEnabled(isPlay);
    ui->btnZoomOut->setEnabled(isPlay);
    ui->btnBrightnessContrast->setEnabled(isPlay);
    ui->btnStress->setEnabled(isPlay);
    ui->btnStretchContrast->setEnabled(isPlay);
}

void MainWindow::EnableDarkfieldUI(bool value)
{
    ui->editDarkfieldQuantity->setEnabled(value);
    ui->btnDarkfieldCapture->setEnabled(value);
}

void MainWindow::EnableCoolingUI(bool value)
{
    ui->sliderTemperature->setEnabled(value);
    ui->editTemperature->setEnabled(value);
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
        // Handle RGB24 case
        Miicam_put_Option(miiHcam, MIICAM_OPTION_RGB, 0);
    }
    else if (index == static_cast<int>(PixelFormatType::RGB32))
    {
        Miicam_put_Option(miiHcam, MIICAM_OPTION_RGB, 2);
    }
    else if (index == static_cast<int>(PixelFormatType::Raw))
    {
        Miicam_put_Option(miiHcam, MIICAM_OPTION_RAW, 1);
    }
}

void MainWindow::btnPlayCamera_Click()
{
    // camera가 run이 아니었으면 시작
    if (!this->isCameraRun)
    {
        MainWindow::StartCamera();
        this->isCameraPlay = true;

        ui->btnPlayCamera->setText(MENU_PAUSE);

        MainWindow::EnablePreviewUI(true);
    }
    else
    {
        this->isCameraPlay = !this->isCameraPlay;

        if (this->isCameraPlay)
        {
            // resume camera
            Miicam_Pause(this->miiHcam, 0);  /* 1 => pause, 0 => continue */
            ui->btnPlayCamera->setText(MENU_PAUSE);
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
    if (!this->resultImage.isNull())
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

        this->resultImage.save(filePath);
    }
}

void MainWindow::chkRecord_CheckedChanged(Qt::CheckState checkState)
{
    if (checkState == Qt::CheckState::Checked)
    {
        this->isRecordOn = true;
        this->videoFrames.clear();

        this->recordStartTime = QTime::currentTime();
        this->timerVideoRecord->start(1000); // Update every second
    }
    else if (checkState == Qt::CheckState::Unchecked)
    {
        this->isRecordOn = false;
        if (this->videoFrames.size() > 0)
        {
            QString pathDir = QCoreApplication::applicationDirPath() + DIR_RECORD_VIDEO;

            QDir dir(pathDir);
            if (!dir.exists())
            {
                dir.mkpath(pathDir);
            }

            QDateTime currentDateTime = QDateTime::currentDateTime();
            QString timestamp = currentDateTime.toString(FORMAT_DATE_TIME);
            QString filePath = dir.absoluteFilePath(timestamp + this->recordFormatExtension);

            cv::VideoWriter writer(filePath.toStdString(), this->recordFormat, FRAME_PER_SECOND, cv::Size(this->videoFrames[0].cols, this->videoFrames[0].rows));

            for (const cv::Mat& mat : this->videoFrames)
            {
                writer.write(mat);
            }

            writer.release();

            this->videoFrames.clear();
        }
    }
}

void MainWindow::btnRecordOption_Click()
{

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

}

void MainWindow::cbCurvePreset_SelectedIndexChanged(int index)
{

}

void MainWindow::chkDarkfield_CheckedChanged(Qt::CheckState checkState)
{
    MainWindow::EnableDarkfieldUI(ui->chkDarkfield->isChecked());
}

void MainWindow::btnDarkfieldCapture_Click()
{

}

void MainWindow::editDarkfieldQuantity_editingFinished()
{
    // bool ok;
    // double value = ui->editExposureTime->toPlainText().toDouble(&ok);

    // if (ok)
    // {
    //     // trackbar에는 정수로 들어가야 하므로 10을 곱한다.
    //     int valueInt = (int)(value * 10.0);

    //     if (valueInt >= ui->sliderExposureTime->minimum() && valueInt <= ui->sliderExposureTime->maximum())
    //     {
    //         // slider에도 값 업데이트
    //         ui->sliderExposureTime->setValue(valueInt);
    //     }
    //     else
    //     {
    //         QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_RANGE);

    //         // 기존 값으로 되돌린다.
    //         value = roundToDecimalPlaces(ui->sliderExposureTime->value() * 0.1, 1);
    //         ui->editExposureTime->setPlainText(QString::number(value, 'f', 1));
    //     }
    // }
    // else
    // {
    //     QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_VALUE);

    //     // 기존 값으로 되돌린다.
    //     value = roundToDecimalPlaces(ui->sliderExposureTime->value() * 0.1, 1);
    //     ui->editExposureTime->setPlainText(QString::number(value, 'f', 1));
    // }
}

void MainWindow::btnGroupCooling_Click(int id)
{
    MainWindow::EnableCoolingUI(id == 1);
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
    zoomFactor += ZOOM_VALUE;

    if (zoomFactor > ZOOM_MAX)
    {
        zoomFactor = ZOOM_MAX;
    }

    ui->gvPreview->scale(zoomFactor, zoomFactor);
}

void MainWindow::btnZoomOut_Click()
{
    zoomFactor -= ZOOM_VALUE;

    if (zoomFactor < ZOOM_MIN)
    {
        zoomFactor = ZOOM_MIN;
    }

    ui->gvPreview->scale(zoomFactor, zoomFactor);
}

void MainWindow::btnBrightnessContrast_Click()
{
    dialog_brightness_contrast dialog(this->isUpdateBrightnessContrast, this->gegl_brightness, this->gegl_contrast, this);

    if (dialog.exec() == QDialog::Accepted)
    {
        this->isUpdateBrightnessContrast = dialog.getEnable();
        this->gegl_brightness = dialog.getBrightness();
        this->gegl_contrast = dialog.getContrast();
    }
}

void MainWindow::btnStress_Click()
{
    dialog_stress dialog(this->isUpdateStress, this->gegl_stress_radius, this->gegl_stress_samples, this->gegl_stress_iterations, this->gegl_stress_enhance_shadows, this);

    if (dialog.exec() == QDialog::Accepted)
    {
        this->isUpdateStress = dialog.getEnable();
        this->gegl_stress_radius = dialog.getRadius();
        this->gegl_stress_samples = dialog.getSamples();
        this->gegl_stress_iterations = dialog.getIterations();
        this->gegl_stress_enhance_shadows = dialog.getEnhanceShadows();
    }
}

void MainWindow::btnStretchContrast_Click()
{
    dialog_stretch_contrast dialog(this->isUpdateStretchContrast, this->gegl_stretch_contrast_keep_colors, this->gegl_stretch_contrast_perceptual);

    if (dialog.exec() == QDialog::Accepted)
    {
        this->isUpdateStretchContrast = dialog.getEnable();
        this->gegl_stretch_contrast_keep_colors = dialog.getKeepColors();
        this->gegl_stretch_contrast_perceptual = dialog.getNonLinearComponents();
    }
}

/////////////////////// video
void MainWindow::btnLoadVideo_Click()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Video"), "", tr("Video Files (*.mp4 *.avi *.mkv)"));

    if (!fileName.isEmpty())
    {
        mpVideoFile->setSource(QUrl::fromLocalFile(fileName));
        ui->lbPathVideo->setText(fileName);

        ui->btnPlayVideo->setEnabled(true);
        ui->btnStopVideo->setEnabled(true);

        // 일단 자동으로 시작하게 한다.
        mpVideoFile->play();
        MainWindow::SetPlayVideo(true);
    }
}

void MainWindow::btnPlayVideo_Click()
{
    if (isVideoPlay)
    {
        mpVideoFile->pause();
        MainWindow::SetPlayVideo(false);
    }
    else
    {
        mpVideoFile->play();
        MainWindow::SetPlayVideo(true);
    }
}


void MainWindow::btnStopVideo_Click()
{
    mpVideoFile->stop();
    MainWindow::SetPlayVideo(false);
}

void MainWindow::onVideoStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia)
    {
        MainWindow::SetPlayVideo(false);
    }
}


/////////////////////// frame
void MainWindow::btnLoadFrame_Click()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty())
    {
        ui->lvFrames->setRootIndex(this->modelFrames->setRootPath(dir));
        ui->lbDirFrames->setText(dir);
    }
}

void MainWindow::lvFrames_Click(const QModelIndex &index)
{
    QString filePath = this->modelFrames->filePath(index);
    QImage image(filePath);
    ui->gvFrameCapture->setImage(image);
    ui->gvFrameCapture->fitInView();
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
            QImage source = QImage(this->rawCameraData, this->rawCameraWidth, this->rawCameraHeight, QImage::Format_RGB888);

            // gegl에서는 rgba를 받기 때문에 무조건 rgba로 바꿔야 한다.
            QImage formattedSource = source.convertToFormat(QImage::Format_RGBA8888);

            if (this->isUpdateBrightnessContrast)
            {
                UpdateBrightnessContrast(formattedSource, this->gegl_brightness, this->gegl_contrast);
            }

            if (this->isUpdateStress)
            {
                UpdateStress(formattedSource, this->gegl_stress_radius, this->gegl_stress_samples, this->gegl_stress_iterations, this->gegl_stress_enhance_shadows);
            }

            if (this->isUpdateStretchContrast)
            {
                UpdateStretchContrast(formattedSource, this->gegl_stretch_contrast_keep_colors, this->gegl_stretch_contrast_perceptual);
            }

            if (this->isUpdateContrastCurve)
            {
                UpdateContrastCurve(formattedSource, this->contrastCurves, this->gegl_contrast_curve_sampling_points);
            }

            // 원래 format으로 되돌린다.
            source = formattedSource.convertToFormat(QImage::Format_RGB888);

            // gegl을 적용한 후에 result에 넣는다. 그래야 video나 capture에서 gegl이 적용된 이미지가 사용될 수 있음.
            this->resultImage = source;

            if (this->isRecordOn)
            {
                cv::Mat mat(source.height(), source.width(), CV_8UC3, const_cast<uchar*>(source.bits()), source.bytesPerLine());
                cv::Mat matBGR;
                cv::cvtColor(mat, matBGR, cv::COLOR_RGB2BGR);  // rgb -> bgr

                this->videoFrames.emplace_back(matBGR);
            }

            QMetaObject::invokeMethod(this, "UpdateGraphicsView", Qt::QueuedConnection);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_PER_SECOND)); // Frame rate delay
    }
}

void MainWindow::UpdateGraphicsView()
{
    ui->gvPreview->setImage(this->resultImage);

    if (std::abs(zoomFactor - 1.0f) <= std::numeric_limits<float>::epsilon())
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
    }
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
        delete[] this->contrastCurves;
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

        Miicam_put_Option(this->miiHcam, MIICAM_OPTION_BYTEORDER, 0); //Qimage use RGB byte order
        Miicam_put_AutoExpoEnable(this->miiHcam, 1);
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
    double MIICAM_EXPOSURE_TIME_MIN = 0.1;
    double MIICAM_EXPOSURE_TIME_MAX = 5000.0;

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
    ui->editExposureTime->setPlainText(QString::number(round(value)));
}

void MainWindow::UpdateSensorTemperature()
{
    double MIICAM_TEMPERATURE_MIN = -50.0;
    double MIICAM_TEMPERATURE_MAX = 40.0;

    short temperature;
    Miicam_get_Temperature(this->miiHcam, &temperature);

    // temperature는 3.2도를 32로 받기 때문에 0.1을 곱한다.
    double value = temperature * 0.1;

    ui->sliderTemperature->setMinimum((int)(MIICAM_TEMPERATURE_MIN * 10.0));
    ui->sliderTemperature->setMaximum((int)(MIICAM_TEMPERATURE_MAX * 10.0));
    ui->sliderTemperature->setValue((int)(value * 10.0));
    ui->editTemperature->setPlainText(QString::number(round(value)));
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


void MainWindow::SetPlayVideo(bool value)
{
    if (!value)
    {
        ui->btnPlayVideo->setText(BTN_PLAY);
        isVideoPlay = false;
    }
    else
    {
        ui->btnPlayVideo->setText(MENU_PAUSE);
        isVideoPlay = true;
    }
}

void MainWindow::UpdateBrightnessContrast(QImage& source, const double brightness, const double contrast)
{
    // Create GEGL buffers
    GeglBuffer* input_buffer = gegl_buffer_new(GEGL_RECTANGLE(0, 0, source.width(), source.height()), babl_format("R'G'B'A u8"));
    GeglBuffer* output_buffer = gegl_buffer_new(GEGL_RECTANGLE(0, 0, source.width(), source.height()), babl_format("R'G'B'A u8"));

    // Set input buffer data
    gegl_buffer_set(input_buffer, nullptr, 0, babl_format("R'G'B'A u8"), source.bits(), GEGL_AUTO_ROWSTRIDE);

    // Create GEGL graph
    GeglNode* graph = gegl_node_new();
    GeglNode* input = gegl_node_new_child(graph, "operation", "gegl:buffer-source", "buffer", input_buffer, nullptr);
    GeglNode* action = gegl_node_new_child(graph, "operation", "gegl:brightness-contrast", nullptr);
    GeglNode* output = gegl_node_new_child(graph, "operation", "gegl:write-buffer", "buffer", output_buffer, nullptr);

    // Set brightness and contrast
    gegl_node_set(action, "brightness", brightness, "contrast", contrast, nullptr);
    gegl_node_link_many(input, action, output, nullptr);
    gegl_node_process(output);

    // Get output buffer data
    gegl_buffer_get(output_buffer, nullptr, 1.0, babl_format("R'G'B'A u8"), source.bits(), GEGL_AUTO_ROWSTRIDE, GEGL_ABYSS_NONE);

    // Unreference GEGL buffers and graph
    g_object_unref(input_buffer);
    g_object_unref(output_buffer);
    g_object_unref(graph);
}

void MainWindow::UpdateStress(QImage& source, const int radius, const int samples, const int iterations, const bool enhanceShadows)
{
    GeglBuffer* input_buffer = gegl_buffer_new(GEGL_RECTANGLE(0, 0, source.width(), source.height()), babl_format("R'G'B'A u8"));
    GeglBuffer* output_buffer = gegl_buffer_new(GEGL_RECTANGLE(0, 0, source.width(), source.height()), babl_format("R'G'B'A u8"));

    gegl_buffer_set(input_buffer, nullptr, 0, babl_format("R'G'B'A u8"), source.bits(), GEGL_AUTO_ROWSTRIDE);

    GeglNode* graph = gegl_node_new();
    GeglNode* input = gegl_node_new_child(graph, "operation", "gegl:buffer-source", "buffer", input_buffer, nullptr);
    GeglNode* action = gegl_node_new_child(graph, "operation", "gegl:stress", nullptr);
    GeglNode* output = gegl_node_new_child(graph, "operation", "gegl:write-buffer", "buffer", output_buffer, nullptr);

    gegl_node_set(action, "radius", radius, "samples", samples, "iterations", iterations, "enhance-shadows", enhanceShadows, nullptr);
    gegl_node_link_many(input, action, output, nullptr);
    gegl_node_process(output);
    gegl_buffer_get(output_buffer, nullptr, 1.0, babl_format("R'G'B'A u8"), source.bits(), GEGL_AUTO_ROWSTRIDE, GEGL_ABYSS_NONE);

    g_object_unref(input_buffer);
    g_object_unref(output_buffer);
    g_object_unref(graph);
}

void MainWindow::UpdateStretchContrast(QImage& source, const bool keepColors, const bool perceptual)
{
    GeglBuffer* input_buffer = gegl_buffer_new(GEGL_RECTANGLE(0, 0, source.width(), source.height()), babl_format("R'G'B'A u8"));
    GeglBuffer* output_buffer = gegl_buffer_new(GEGL_RECTANGLE(0, 0, source.width(), source.height()), babl_format("R'G'B'A u8"));

    gegl_buffer_set(input_buffer, nullptr, 0, babl_format("R'G'B'A u8"), source.bits(), GEGL_AUTO_ROWSTRIDE);

    GeglNode* graph = gegl_node_new();
    GeglNode* input = gegl_node_new_child(graph, "operation", "gegl:buffer-source", "buffer", input_buffer, nullptr);
    GeglNode* action = gegl_node_new_child(graph, "operation", "gegl:stretch-contrast", nullptr);
    GeglNode* output = gegl_node_new_child(graph, "operation", "gegl:write-buffer", "buffer", output_buffer, nullptr);

    gegl_node_set(action, "keep-colors", keepColors, "perceptual", perceptual, nullptr);
    gegl_node_link_many(input, action, output, nullptr);
    gegl_node_process(output);
    gegl_buffer_get(output_buffer, nullptr, 1.0, babl_format("R'G'B'A u8"), source.bits(), GEGL_AUTO_ROWSTRIDE, GEGL_ABYSS_NONE);

    g_object_unref(input_buffer);
    g_object_unref(output_buffer);
    g_object_unref(graph);
}

void MainWindow::UpdateContrastCurve(QImage& source, const GeglCurve* curve, const int samplingPoints)
{
    GeglBuffer* input_buffer = gegl_buffer_new(GEGL_RECTANGLE(0, 0, source.width(), source.height()), babl_format("R'G'B'A u8"));
    GeglBuffer* output_buffer = gegl_buffer_new(GEGL_RECTANGLE(0, 0, source.width(), source.height()), babl_format("R'G'B'A u8"));

    gegl_buffer_set(input_buffer, nullptr, 0, babl_format("R'G'B'A u8"), source.bits(), GEGL_AUTO_ROWSTRIDE);

    GeglNode* graph = gegl_node_new();
    GeglNode* input = gegl_node_new_child(graph, "operation", "gegl:buffer-source", "buffer", input_buffer, nullptr);
    GeglNode* action = gegl_node_new_child(graph, "operation", "gegl:contrast-curve", nullptr);
    GeglNode* output = gegl_node_new_child(graph, "operation", "gegl:write-buffer", "buffer", output_buffer, nullptr);

    // 커브 설정 sample
    //GeglCurve* curve = gegl_curve_new(0.0, 1.0);
    //gegl_curve_add_point(curve, 0.0, 0.0);
    //gegl_curve_add_point(curve, 0.25, 0.2);
    //gegl_curve_add_point(curve, 0.5, 0.5);
    //gegl_curve_add_point(curve, 0.75, 0.8);
    //gegl_curve_add_point(curve, 1.0, 1.0);

    gegl_node_set(action, "curve", curve, "sampling-points", samplingPoints, nullptr);
    gegl_node_link_many(input, action, output, nullptr);
    gegl_node_process(output);
    gegl_buffer_get(output_buffer, nullptr, 1.0, babl_format("R'G'B'A u8"), source.bits(), GEGL_AUTO_ROWSTRIDE, GEGL_ABYSS_NONE);

    g_object_unref(input_buffer);
    g_object_unref(output_buffer);
    g_object_unref(graph);
}


