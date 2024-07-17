#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "constants.h"
#include "PixelFormatType.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , modelFrames(new QFileSystemModel(this))
    , mpVideoFile(new QMediaPlayer(this))
    , timer(new QTimer(this))
    , captureDir(QCoreApplication::applicationDirPath() + PATH_CAPTURE)
{
    ui->setupUi(this);

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


    // ui 초기화 후에 우선 카메라부터 찾는다.
    MainWindow::FindCamera();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent*)
{
    CloseCamera();
}

void MainWindow::cbResoution_SelectedIndexChanged(int index)
{
    resolutionIndex = index;
    imageWidth = miiDevice.model->res[index].width;
    imageHeight = miiDevice.model->res[index].height;

    if (miiHcam) //step 1: stop camera
    {
        Miicam_Stop(miiHcam);
    }

    Miicam_put_eSize(miiHcam, static_cast<unsigned>(resolutionIndex));

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

void MainWindow::chkRecord_CheckedChanged()
{

}

void MainWindow::btnPlayCamera_Click()
{
    // camera가 run이 아니었으면 시작
    if (!isCameraRun)
    {
        StartCamera();
        isCameraPlay = true;

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
        isCameraPlay = !isCameraPlay;

        if (isCameraPlay)
        {
            // resume camera
            Miicam_Pause(miiHcam, 0);  /* 1 => pause, 0 => continue */
            ui->btnPlayCamera->setText(MENU_PAUSE);
        }
        else
        {
            // pause camera
            Miicam_Pause(miiHcam, 1);  /* 1 => pause, 0 => continue */
            ui->btnPlayCamera->setText(MENU_PLAY);
        }
    }
}


void MainWindow::btnStopCamera_Click()
{
    Miicam_Stop(miiHcam);
    isCameraRun = false;

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
    Miicam_Snap(miiHcam, resolutionIndex);
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

void MainWindow::FindCamera()
{
    if (miiHcam)
    {
        CloseCamera();
    }
    else
    {
        MiicamDeviceV2 arr[MIICAM_MAX] = { 0 };
        unsigned count = Miicam_EnumV2(arr);
        if (count > 0)
        {
            miiDevice = arr[0];

            {
                const QSignalBlocker blocker(ui->lbDeviceName);
                ui->lbDeviceName->setText(QString::fromWCharArray(miiDevice.displayname));
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
    miiHcam = Miicam_Open(miiDevice.id);

    if (miiHcam)
    {
        Miicam_get_eSize(miiHcam, (unsigned*)&resolutionIndex);
        imageWidth = miiDevice.model->res[resolutionIndex].width;
        imageHeight = miiDevice.model->res[resolutionIndex].height;

        // open에 성공하면 resolution 업데이트
        {
            const QSignalBlocker blocker(ui->cbResolution);
            ui->cbResolution->clear();
            for (unsigned i = 0; i < miiDevice.model->preview; ++i)
            {
                ui->cbResolution->addItem(QString::asprintf("%u x %u", miiDevice.model->res[i].width, miiDevice.model->res[i].height));
            }
            ui->cbResolution->setCurrentIndex(resolutionIndex);

            ui->cbResolution->setEnabled(true);
            ui->cbFormat->setEnabled(true);

            ui->btnPlayCamera->setEnabled(true);
        }

        Miicam_put_Option(miiHcam, MIICAM_OPTION_BYTEORDER, 0); //Qimage use RGB byte order
        Miicam_put_AutoExpoEnable(miiHcam, 1);
    }
}

void MainWindow::CloseCamera()
{
    if (miiHcam)
    {
        Miicam_Close(miiHcam);
        miiHcam = nullptr;
    }
    delete[] imageData;
    imageData = nullptr;

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
    if (imageData)
    {
        delete[] imageData;
        imageData = nullptr;
    }

    imageData = new uchar[TDIBWIDTHBYTES(imageWidth * 24) * imageHeight];
    unsigned uimax = 0, uimin = 0, uidef = 0;
    unsigned short usmax = 0, usmin = 0, usdef = 0;
    Miicam_get_ExpTimeRange(miiHcam, &uimin, &uimax, &uidef);

    //m_slider_expoTime->setRange(uimin, uimax);
    Miicam_get_ExpoAGainRange(miiHcam, &usmin, &usmax, &usdef);
    //m_slider_expoGain->setRange(usmin, usmax);
    if (0 == (miiDevice.model->flag & MIICAM_FLAG_MONO))
    {
        handleTempTintEvent();
    }
    handleExpoEvent();

    if (SUCCEEDED(Miicam_StartPullModeWithCallback(miiHcam, eventCallBack, this)))
    {
        isCameraRun = true;

        //m_cmb_res->setEnabled(true);
        //m_cbox_auto->setEnabled(true);
        //m_btn_autoWB->setEnabled(0 == (m_cur.model->flag & MIICAM_FLAG_MONO));
        //m_slider_temp->setEnabled(0 == (m_cur.model->flag & MIICAM_FLAG_MONO));
        //m_slider_tint->setEnabled(0 == (m_cur.model->flag & MIICAM_FLAG_MONO));
        //m_btn_open->setText("Close");
        //m_btn_snap->setEnabled(true);

        int bAuto = 0;
        Miicam_get_AutoExpoEnable(miiHcam, &bAuto);
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

    if (miiHcam && SUCCEEDED(Miicam_get_FrameRate(miiHcam, &nFrame, &nTime, &nTotalFrame)) && (nTime > 0))
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
    if (miiHcam)
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
        else if (MIICAM_EVENT_STILLIMAGE == nEvent)
        {
            handleStillImageEvent();
        }
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
    unsigned width = 0, height = 0;
    if (SUCCEEDED(Miicam_PullImage(miiHcam, imageData, 24, &width, &height)))
    {
        // 가만보니 기본 ui도 좀 문제긴 하네.
        QImage image(imageData, width, height, QImage::Format_RGB888);
        QImage newimage = image.scaled(ui->lbPreview->width(), ui->lbPreview->height(), Qt::KeepAspectRatio, Qt::FastTransformation);

        ui->lbPreview->setAlignment(Qt::AlignCenter);
        ui->lbPreview->setPixmap(QPixmap::fromImage(newimage));

        // 이렇게 하면 UI가 자꾸 문제가 되서 일단 위의 것으로 함
        // QImage image(imageData, width, height, QImage::Format_RGB888);

        // int scaledWidth = ui->lbPreview->width() * zoomFactor;
        // int scaledHeight = ui->lbPreview->height() * zoomFactor;
        // QImage newimage = image.scaled(scaledWidth, scaledHeight, Qt::KeepAspectRatio, Qt::FastTransformation);

        // //ui->lbPreview->setAlignment(Qt::AlignCenter);
        // //ui->lbPreview->setPixmap(QPixmap::fromImage(newimage));


        // // QLabel 크기 얻기
        // int labelWidth = ui->lbPreview->width();
        // int labelHeight = ui->lbPreview->height();

        // // QPixmap 생성 및 QPainter로 그리기
        // QPixmap pixmap(labelWidth, labelHeight);
        // pixmap.fill(Qt::transparent); // 배경 투명하게 설정
        // QPainter painter(&pixmap);
        // int x = (labelWidth - newimage.width()) / 2;
        // int y = (labelHeight - newimage.height()) / 2;
        // painter.drawImage(x, y, newimage);
        // painter.end();

        // ui->lbPreview->setPixmap(pixmap);
    }
}

void MainWindow::handleExpoEvent()
{
    unsigned time = 0;
    unsigned short gain = 0;
    Miicam_get_ExpoTime(miiHcam, &time);
    Miicam_get_ExpoAGain(miiHcam, &gain);

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
    if (SUCCEEDED(Miicam_get_TempTint(miiHcam, &nTemp, &nTint)))
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

void MainWindow::handleStillImageEvent()
{
    unsigned width = 0, height = 0;
    if (SUCCEEDED(Miicam_PullStillImage(miiHcam, nullptr, 24, &width, &height))) // peek
    {
        std::vector<uchar> vec(TDIBWIDTHBYTES(width * 24) * height);
        if (SUCCEEDED(Miicam_PullStillImage(miiHcam, &vec[0], 24, &width, &height)))
        {
            QImage image(&vec[0], width, height, QImage::Format_RGB888);

            // 현재 날짜와 시간을 가져오기
            QDateTime currentDateTime = QDateTime::currentDateTime();
            QString formattedDateTime = currentDateTime.toString("yyyy_MM_dd_hh_mm_ss");

            QString filename = captureDir + QString("/%1.jpg").arg(formattedDateTime);

             // 디렉토리가 존재하지 않으면 생성
            QDir dir(captureDir);
            if (!dir.exists()) {
                dir.mkpath(captureDir);
            }
            image.save(filename);
        }
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
    modelFrames->setRootPath(capturePath);
    modelFrames->setNameFilters(QStringList() << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp" << "*.gif");
    modelFrames->setNameFilterDisables(false);
}

void MainWindow::btnLoadFrame_Click()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty())
    {
        ui->lvFrames->setRootIndex(modelFrames->setRootPath(dir));
        ui->lbDirFrames->setText(dir);
    }
}

void MainWindow::onSelecteImage(const QModelIndex &index)
{
    QString filePath = modelFrames->filePath(index);
    QPixmap pixmap(filePath);
    ui->lbFrameCapture->setPixmap(pixmap.scaled(ui->lbFrameCapture->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

// void MainWindow::btnBrightnessContrast_Click()
// {
//     QImage originalImage;
//     QImage processedImage;

//     QString filePath = "D:/Freelancer/iiSM-SW_public/SIMP/SIMP/build/Desktop_Qt_6_7_2_MinGW_64_bit-Debug/captures/2024_07_17_14_52_18.jpg";
//     originalImage.load(filePath);

//     double contrast = 1.0;
//     double brightness = 0.0;

//     gegl_init(nullptr, nullptr);

//     GeglNode *graph = gegl_node_new();
//     GeglNode *load = gegl_node_new_child(graph, "gegl:load", "path", &originalImage, nullptr);
//     GeglNode *brightnessContrast = gegl_node_new_child(graph, "gegl:brightness-contrast", "brightness", brightness, "contrast", contrast, nullptr);
//     GeglNode *save = gegl_node_new_child(graph, "gegl:save", "path", &processedImage, nullptr);

//     gegl_node_link_many(load, brightnessContrast, save, nullptr);
//     gegl_node_process(save);

//     // Unref the nodes
//     //gegl_node_unref(load);
//     //gegl_node_unref(brightnessContrast);
//     //gegl_node_unref(save);
//     //gegl_node_unref(graph);

//     gegl_exit();

//     ui->lbFrameCapture->setPixmap(QPixmap::fromImage(processedImage));
// }

void MainWindow::btnBrightnessContrast_Click()
{

    // QImage originalImage;
    // QImage processedImage;

    // QString filePath = "D:/Freelancer/iiSM-SW_public/SIMP/SIMP/build/Desktop_Qt_6_7_2_MinGW_64_bit-Debug/captures/2024_07_17_14_52_18.jpg";
    // originalImage.load(filePath);

    // double contrast = 1.0;
    // double brightness = 0.0;

    // gegl_init(nullptr, nullptr);

    // GeglNode *graph = gegl_node_new();
    // GeglNode *load = gegl_node_new_child(graph, "gegl:load", "path", filePath.toStdString().c_str(), nullptr);
    // GeglNode *brightnessContrast = gegl_node_new_child(graph, "gegl:brightness-contrast", "brightness", brightness, "contrast", contrast, nullptr);
    // GeglNode *save = gegl_node_new_child(graph, "gegl:save-buffer", nullptr);

    // gegl_node_link_many(load, brightnessContrast, save, nullptr);
    // gegl_node_process(save);

    // GeglBuffer *buffer = gegl_node_get_buffer(save);
    // if (buffer)
    // {
    //     gint width = gegl_buffer_get_width(buffer);
    //     gint height = gegl_buffer_get_height(buffer);
    //     gint rowstride = width * 4;
    //     processedImage = QImage(width, height, QImage::Format_RGB32);

    //     gegl_buffer_get(buffer, GEGL_RECTANGLE(0, 0, width, height), 1.0, processedImage.bits(), GEGL_AUTO_ROWSTRIDE, GEGL_ABYSS_NONE);
    // }

    // // Unref the nodes
    // gegl_node_unref(load);
    // gegl_node_unref(brightnessContrast);
    // gegl_node_unref(save);
    // gegl_node_unref(graph);

    // gegl_exit();

    // if (!processedImage.isNull())
    // {
    //     ui->lbFrameCapture->setPixmap(QPixmap::fromImage(processedImage));
    // }
    // else
    // {
    //     // Handle error
    //     qDebug() << "Failed to process image";
    // }
}

