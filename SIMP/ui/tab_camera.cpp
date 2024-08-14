#include "tab_camera.h"
#include "ui_tab_camera.h"
#include "simp_util.h"
#include "simp_const_value.h"
#include "simp_const_path.h"
#include "simp_const_menu.h"
#include "simp_const_format.h"

#include <QTimer>
#include <QMessageBox>
#include <QDir>

TabCamera::TabCamera(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TabCamera)
    , recordDir(QCoreApplication::applicationDirPath() + SimpConstPath::DIR_RECORD_VIDEO)
    , captureDir(QCoreApplication::applicationDirPath() + SimpConstPath::DIR_CAPTURE_FRAME)
{
    ui->setupUi(this);
}

TabCamera::~TabCamera()
{
    delete ui;
}


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

    if (SUCCEEDED(Miicam_PullImage(this->miiHcam, this->rawCameraData, 24, &this->rawCameraWidth, &this->rawCameraHeight)))
    {
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
        int fourcc = SimpUtil::getVideoFourcc(this->recordFormat);

        try
        {
            // Create VideoWriter object
            cv::VideoWriter writer(filePath.toStdString(), fourcc, this->recordFrameRate, cv::Size(this->recordFrames[0].cols, this->recordFrames[0].rows));

            // quality는 특정 format에만 적용된다.
            if (this->recordFormat == VideoFormatType::MJPEG)
            {
                writer.set(cv::VIDEOWRITER_PROP_QUALITY, this->recordQuality);
            }

            // Write frames to video file
            for (const cv::Mat& mat : this->recordFrames)
            {
                writer.write(mat);
            }

            // Release the VideoWriter
            writer.release();
        }
        catch (cv::Exception ex)
        {
            QMessageBox::critical(this, SimpConstMenu::TITLE_ERROR, QString::fromStdString(ex.msg));
        }

        this->recordFrames.clear();
    }

    ui->btnRecordOn->setText(SimpConstMenu::BTN_RECORD_ON);
}
