#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "PixelFormatType.h"

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
#include <thread>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , scenePreview(new QGraphicsScene(this))
    , sceneFrame(new QGraphicsScene(this))
    , modelFrames(new QFileSystemModel(this))
    , mpVideoFile(new QMediaPlayer(this))
    , timer(new QTimer(this))
    , captureDir(QCoreApplication::applicationDirPath() + PATH_CAPTURE_FRAME)
{
    ui->setupUi(this);
    ui->gvPreview->setScene(scenePreview);
    ui->gvFrameCapture->setScene(sceneFrame);

    qApp->setStyleSheet(
        "QPushButton:disabled { background-color: lightgray; color: darkgray; }"
        "QComboBox:disabled { background-color: lightgray; color: darkgray; }"
        "QCheckbox:disabled { background-color: lightgray; color: darkgray; }"
        "QPlainTextEdit:disabled { background-color: lightgray; color: darkgray; }"
        "QSlider:disabled { background-color: lightgray; color: darkgray; }"
        "QRadioButton:disabled { background-color: lightgray; color: darkgray; }"
        );


    // tab preview
    connect(this, &MainWindow::evtCallback, this, &MainWindow::onMiiCameraCallback);

    connect(ui->cbResolution, &QComboBox::currentIndexChanged, this, &MainWindow::cbResoution_SelectedIndexChanged);
    connect(ui->cbFormat, &QComboBox::currentIndexChanged, this, &MainWindow::cbFormat_SelectedIndexChanged);

    connect(ui->btnPlayCamera, &QPushButton::clicked, this, &MainWindow::btnPlayCamera_Click);
    connect(ui->btnStopCamera, &QPushButton::clicked, this, &MainWindow::btnStopCamera_Click);
    connect(ui->btnCaptureCamera, &QPushButton::clicked, this, &MainWindow::btnCaptureCamera_Click);
    connect(ui->chkRecord, &QCheckBox::checkStateChanged, this, &MainWindow::chkRecord_CheckedChanged);

    connect(ui->btnZoomIn, &QPushButton::clicked, this, &MainWindow::btnZoomIn_Click);
    connect(ui->btnZoomOut, &QPushButton::clicked, this, &MainWindow::btnZoomOut_Click);

    connect(timer, &QTimer::timeout, this, &MainWindow::onTimerCallback);

    // update combobox - 나중에 옮길 것.
    ui->cbFormat->clear();
    for (const auto& format : {PixelFormatType::RGB24, PixelFormatType::RGB32, PixelFormatType::Raw})
    {
        ui->cbFormat->addItem(toString(format));
    }
    ui->cbFormat->setCurrentIndex(0);

    ui->cbResolution->setEnabled(false);
    ui->cbFormat->setEnabled(false);

    ui->btnPlayCamera->setEnabled(false);
    ui->btnStopCamera->setEnabled(false);
    ui->btnCaptureCamera->setEnabled(false);

    ui->btnZoomIn->setEnabled(false);
    ui->btnZoomOut->setEnabled(false);

    // tab video
    mpVideoFile->setVideoOutput(ui->videoFile);

    connect(mpVideoFile, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onVideoStatusChanged);

    connect(ui->btnLoadVideo, &QPushButton::clicked, this, &MainWindow::btnLoadVideo_Click);
    connect(ui->btnPlayVideo, &QPushButton::clicked, this, &MainWindow::btnPlayVideo_Click);
    connect(ui->btnStopVideo, &QPushButton::clicked, this, &MainWindow::btnStopVideo_Click);

    ui->btnPlayVideo->setEnabled(false);
    ui->btnStopVideo->setEnabled(false);



    // tab frame
    SetupModel(captureDir);

    ui->lvFrames->setModel(modelFrames);
    ui->lvFrames->setRootIndex(modelFrames->index(captureDir)); // Set the root index
    ui->lbDirFrames->setText(captureDir);

    connect(ui->lvFrames, &QListView::clicked, this, &MainWindow::onSelecteImage);

    connect(ui->btnLoadFrame, &QPushButton::clicked, this, &MainWindow::btnLoadFrame_Click);



    // gegl
    connect(ui->btnBrightnessContrast, &QPushButton::clicked, this, &MainWindow::btnBrightnessContrast_Click);
    connect(ui->btnStress, &QPushButton::clicked, this, &MainWindow::btnStress_Click);
    connect(ui->btnStretchContrast, &QPushButton::clicked, this, &MainWindow::btnStretchContrast_Click);

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

    QWidget *currentWidget = ui->tabWidget->currentWidget();

    if (currentWidget == ui->tabPreview)
    {
        if (this->pmiPreview)
        {
            //ui->gvFrameCapture->fitInView(this->pmiPreview, Qt::KeepAspectRatio);
        }
    }
    else if (currentWidget == ui->tabVideo)
    {

    }
    else if (currentWidget == ui->tabCapture)
    {
        if (this->pmiFrame)
        {
            ui->gvFrameCapture->fitInView(this->pmiFrame, Qt::KeepAspectRatio);
        }
    }
}


////////////////////////////////////////
// UI
////////////////////////////////////////

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
        StartCamera();
        this->isCameraPlay = true;

        ui->btnPlayCamera->setText(MENU_PAUSE);
        ui->btnStopCamera->setEnabled(true);
        ui->btnCaptureCamera->setEnabled(true);
        ui->btnZoomIn->setEnabled(true);
        ui->btnZoomOut->setEnabled(true);

        // play가 시작되면 resolution과 format은 변경 불가
        ui->cbResolution->setEnabled(false);
        ui->cbFormat->setEnabled(false);
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
            ui->btnPlayCamera->setText(MENU_PLAY);
        }
    }
}


void MainWindow::btnStopCamera_Click()
{
    Miicam_Stop(this->miiHcam);
    this->isCameraRun = false;

    ui->cbResolution->setEnabled(true);
    ui->cbFormat->setEnabled(true);

    ui->btnPlayCamera->setText(MENU_PLAY);
    ui->btnPlayCamera->setEnabled(true);
    ui->btnStopCamera->setEnabled(false);
    ui->btnCaptureCamera->setEnabled(false);
    ui->btnZoomIn->setEnabled(false);
    ui->btnZoomOut->setEnabled(false);
}


void MainWindow::btnCaptureCamera_Click()
{
    if (!this->resultImage.isNull())
    {
        QString pathDir = QCoreApplication::applicationDirPath() + PATH_CAPTURE_FRAME;

        QDir dir(pathDir);
        if (!dir.exists())
        {
            dir.mkpath(pathDir);
        }

        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString timestamp = currentDateTime.toString("yyyy_MM_dd_hh_mm_ss");
        QString filePath = dir.absoluteFilePath(timestamp + ".png");

        this->resultImage.save(filePath);
    }
}

void MainWindow::chkRecord_CheckedChanged(Qt::CheckState checkState)
{
    if (checkState == Qt::CheckState::Checked)
    {
        this->videoFrames.clear();
    }
    else if (checkState == Qt::CheckState::Unchecked)
    {
        if (this->videoFrames.size() > 0)
        {
            QString pathDir = QCoreApplication::applicationDirPath() + PATH_CAPTURE_FRAME;

            QDir dir(pathDir);
            if (!dir.exists())
            {
                dir.mkpath(pathDir);
            }

            QDateTime currentDateTime = QDateTime::currentDateTime();
            QString timestamp = currentDateTime.toString("yyyy_MM_dd_hh_mm_ss");
            QString filePath = dir.absoluteFilePath(timestamp + this->recordFormatExtension);

            cv::VideoWriter writer(filePath.toStdString(), this->recordFormat, FPS_RECROD_VIDEO, cv::Size(this->videoFrames[0].cols, this->videoFrames[0].rows));

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

void MainWindow::btnZoomIn_Click()
{
    zoomFactor += ZOOM_VALUE;

    if (zoomFactor > ZOOM_MAX)
    {
        zoomFactor = ZOOM_MAX;
    }
}

void MainWindow::btnZoomOut_Click()
{
    zoomFactor -= ZOOM_VALUE;

    if (zoomFactor < ZOOM_MIN)
    {
        zoomFactor = ZOOM_MIN;
    }
}

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

void MainWindow::btnLoadFrame_Click()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty())
    {
        ui->lvFrames->setRootIndex(this->modelFrames->setRootPath(dir));
        ui->lbDirFrames->setText(dir);
    }
}

void MainWindow::onSelecteImage(const QModelIndex &index)
{
    if (this->pmiFrame)
    {
        this->sceneFrame->removeItem(this->pmiFrame);
        delete this->pmiFrame;
    }

    QString filePath = this->modelFrames->filePath(index);
    QPixmap pixmap(filePath);

    this->pmiFrame = this->sceneFrame->addPixmap(pixmap);
    ui->gvFrameCapture->fitInView(this->pmiFrame, Qt::KeepAspectRatio);
}

void MainWindow::btnBrightnessContrast_Click()
{
    this->isUpdateBrightnessContrast = !this->isUpdateBrightnessContrast;
}

void MainWindow::btnStress_Click()
{
    //this->isUpdateStress = !this->isUpdateStress;
}

void MainWindow::btnStretchContrast_Click()
{
    this->isUpdateStretchContrast = !this->isUpdateStretchContrast;
}

void MainWindow::onVideoStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia)
    {
        MainWindow::SetPlayVideo(false);
    }
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
            // 여기서 format은 combobox에서 선택한 format이 되어야 함
            QImage source = QImage(this->rawCameraData, this->rawCameraWidth, this->rawCameraHeight, QImage::Format_RGB888);

            // gegl에서는 rgba를 받기 때문에 무조건 rgba로 바꿔야 한다.
            QImage formattedSource = source.convertToFormat(QImage::Format_RGBA8888);

            if (this->isUpdateBrightnessContrast)
            {
                UpdateBrightnessContrast(formattedSource, this->brightness, this->contrast);
            }

            if (this->isUpdateStress)
            {
                UpdateStress(formattedSource, this->stressRadius, this->stressSamples, this->stressIterations, this->stressEnhanceShadows);
            }

            if (this->isUpdateStretchContrast)
            {
                UpdateStretchContrast(formattedSource, this->stretchContrastKeepColors, this->stretchContrastPerceptual);
            }

            if (this->isUpdateContrastCurve)
            {
                UpdateContrastCurve(formattedSource, this->contrastCurves, this->contrastCurveSamplingPoints);
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
    if (this->pmiPreview)
    {
        this->scenePreview->removeItem(this->pmiPreview);
        delete this->pmiPreview;
    }

    {
        const QSignalBlocker blocker(ui->gvFrameCapture);
        this->pmiPreview = this->scenePreview->addPixmap(QPixmap::fromImage(this->resultImage));
        ui->gvFrameCapture->fitInView(this->pmiPreview, Qt::KeepAspectRatio);
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

            OpenCamera();
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
            ui->cbResolution->clear();
            for (unsigned i = 0; i < this->miiDevice.model->preview; ++i)
            {
                ui->cbResolution->addItem(QString::asprintf("%u x %u", this->miiDevice.model->res[i].width, this->miiDevice.model->res[i].height));
            }
            ui->cbResolution->setCurrentIndex(this->resolutionIndex);

            ui->cbResolution->setEnabled(true);
            ui->cbFormat->setEnabled(true);

            ui->btnPlayCamera->setEnabled(true);
        }

        Miicam_put_Option(this->miiHcam, MIICAM_OPTION_BYTEORDER, 0); //Qimage use RGB byte order
        Miicam_put_AutoExpoEnable(this->miiHcam, 1);
    }
}

void MainWindow::CloseCamera()
{
    if (this->miiHcam)
    {
        Miicam_Close(this->miiHcam);
        this->miiHcam = nullptr;
    }

    if (this->rawCameraData)
    {
        delete[] this->rawCameraData;
    }

    if (this->pmiPreview)
    {
        delete this->pmiPreview;
        this->pmiPreview = nullptr;
    }

    if (this->pmiFrame)
    {
        delete this->pmiFrame;
        this->pmiFrame = nullptr;
    }

    // m_btn_open->setText("Open");
    // m_timer->stop();
    // m_lbl_frame->clear();
    // m_cbox_auto->setEnabled(false);
    // m_slider_expoGain->setEnabled(false);
    // m_slider_expoTime->setEnabled(false);
    // m_btn_autoWB->setEnabled(false);
    // m_slider_temp->setEnabled(false);
    // m_slider_tint->setEnabled(false);
    // m_btn_snap->setEnabled(false);
    // m_cmb_res->setEnabled(false);
    // m_cmb_res->clear();
}

void MainWindow::StartCamera()
{
    if (this->rawCameraData)
    {
        delete[] this->rawCameraData;
        this->rawCameraData = nullptr;
    }

    this->rawCameraData = new uchar[TDIBWIDTHBYTES(imageWidth * 24) * this->imageHeight];

    unsigned uimax = 0, uimin = 0, uidef = 0;
    unsigned short usmax = 0, usmin = 0, usdef = 0;
    Miicam_get_ExpTimeRange(this->miiHcam, &uimin, &uimax, &uidef);

    //m_slider_expoTime->setRange(uimin, uimax);
    Miicam_get_ExpoAGainRange(this->miiHcam, &usmin, &usmax, &usdef);
    //m_slider_expoGain->setRange(usmin, usmax);
    if ((this->miiDevice.model->flag & MIICAM_FLAG_MONO) == 0)
    {
        handleTempTintEvent();
    }
    handleExpoEvent();

    if (SUCCEEDED(Miicam_StartPullModeWithCallback(this->miiHcam, eventCallBack, this)))
    {
        this->isCameraRun = true;

        //m_cmb_res->setEnabled(true);
        //m_cbox_auto->setEnabled(true);
        //m_btn_autoWB->setEnabled(0 == (m_cur.model->flag & MIICAM_FLAG_MONO));
        //m_slider_temp->setEnabled(0 == (m_cur.model->flag & MIICAM_FLAG_MONO));
        //m_slider_tint->setEnabled(0 == (m_cur.model->flag & MIICAM_FLAG_MONO));
        //m_btn_open->setText("Close");
        //m_btn_snap->setEnabled(true);

        int bAuto = 0;
        Miicam_get_AutoExpoEnable(this->miiHcam, &bAuto);
        //m_cbox_auto->setChecked(1 == bAuto);

        // fps update
        timer->start(1000);
    }
    else
    {
        CloseCamera();
        QMessageBox::warning(this, "Warning", "Failed to start camera.");
    }
}

void MainWindow::onTimerCallback()
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
        ui->btnPlayVideo->setText(MENU_PLAY);
        isVideoPlay = false;
    }
    else
    {
        ui->btnPlayVideo->setText(MENU_PAUSE);
        isVideoPlay = true;
    }
}

void MainWindow::SetupModel(const QString& capturePath)
{
    // Check if the captures directory exists, and create it if it doesn't
    QDir dir(capturePath);
    if (!dir.exists()) {
        dir.mkpath(capturePath);
    }

    // Set model properties
    this->modelFrames->setRootPath(capturePath);
    this->modelFrames->setNameFilters(QStringList() << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp" << "*.gif");
    this->modelFrames->setNameFilterDisables(false);
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


