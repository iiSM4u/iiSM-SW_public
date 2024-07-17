#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , modelFrames(new QFileSystemModel(this))
    , mpVideoFile(new QMediaPlayer(this))
    //, miiHcam(nullptr)
    //, pData(nullptr)
    //, imgWidth(0)
    //, imgHeight(0)
{
    ui->setupUi(this);

    // Get the path of the current executable
    QString capturesPath = QCoreApplication::applicationDirPath() + "/captures";
    setupModel(capturesPath);

    // // tab preview
    // connect(ui->btnZoomIn, &QPushButton::clicked, this, &MainWindow::onClickConnectCamera);


    // connect(this, &MainWindow::evtCallback, this, [this](unsigned nEvent)
    //         {
    //             /* this run in the UI thread */
    //             if (miiHcam)
    //             {
    //                 if (MIICAM_EVENT_IMAGE == nEvent)
    //                 {
    //                     handleImageEvent();
    //                 }
    //                 else if (MIICAM_EVENT_EXPOSURE == nEvent)
    //                 {
    //                     handleExpoEvent();
    //                 }
    //                 else if (MIICAM_EVENT_TEMPTINT == nEvent)
    //                 {
    //                     handleTempTintEvent();
    //                 }
    //                 else if (MIICAM_EVENT_STILLIMAGE == nEvent)
    //                 {
    //                     handleStillImageEvent();
    //                 }
    //                 else if (MIICAM_EVENT_ERROR == nEvent)
    //                 {
    //                     closeCamera();
    //                     QMessageBox::warning(this, "Warning", "Generic error.");
    //                 }
    //                 else if (MIICAM_EVENT_DISCONNECTED == nEvent)
    //                 {
    //                     closeCamera();
    //                     QMessageBox::warning(this, "Warning", "Camera disconnect.");
    //                 }
    //             }
    //         });

    // connect(timer, &QTimer::timeout, this, [this]()
    //         {
    //             unsigned nFrame = 0, nTime = 0, nTotalFrame = 0;
    //             if (miiHcam && SUCCEEDED(Miicam_get_FrameRate(miiHcam, &nFrame, &nTime, &nTotalFrame)) && (nTime > 0))
    //                 ui->lbFPS->setText(QString::asprintf("%u, fps = %.1f", nTotalFrame, nFrame * 1000.0 / nTime));
    //         });

    // tab video
    mpVideoFile->setVideoOutput(ui->videoFile);

    connect(ui->btnLoadVideo, &QPushButton::clicked, this, &MainWindow::onClickOpenVideo);
    connect(ui->btnPlayVideo, &QPushButton::clicked, this, &MainWindow::onClickPlayVideo);
    connect(ui->btnStopVideo, &QPushButton::clicked, this, &MainWindow::onClickStopVideo);

    connect(mpVideoFile, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onVideoStatusChanged);

    ui->btnPlayVideo->setEnabled(false);
    ui->btnStopVideo->setEnabled(false)

    // Set up the QListView
    ui->lvFrames->setModel(modelFrames);
    ui->lvFrames->setRootIndex(modelFrames->index(capturesPath)); // Set the root index
    ui->lbDirFrames->setText(capturesPath);
    connect(ui->lvFrames, &QListView::clicked, this, &MainWindow::onSelecteImage);
    connect(ui->btnLoadFrame, &QPushButton::clicked, this, &MainWindow::onClickLoadFrames);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent*)
{
    //closeCamera();
}

void MainWindow::setupModel(const QString& capturesPath)
{
    // Check if the captures directory exists, and create it if it doesn't
    QDir dir(capturesPath);
    if (!dir.exists()) {
        dir.mkpath(capturesPath);
    }

    // Set model properties
    modelFrames->setRootPath(capturesPath);
    modelFrames->setNameFilters(QStringList() << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp" << "*.gif");
    modelFrames->setNameFilterDisables(false);
}

// void MainWindow::onClickConnectCamera()
// {
//     if (miiHcam) //step 1: stop camera
//         Miicam_Stop(miiHcam);

//     int m_res = 0;
//     //m_imgWidth = m_cur.model->res[index].width;
//     //m_imgHeight = m_cur.model->res[index].height;

//     if (miiHcam) //step 2: restart camera
//     {
//         Miicam_put_eSize(miiHcam, static_cast<unsigned>(m_res));
//         startCamera();
//     }
// }

// void MainWindow::startCamera()
// {
//     if (pData)
//     {
//         delete[] pData;
//         pData = nullptr;
//     }

//     pData = new uchar[TDIBWIDTHBYTES(imgWidth * 24) * imgHeight];
//     unsigned uimax = 0, uimin = 0, uidef = 0;
//     unsigned short usmax = 0, usmin = 0, usdef = 0;
//     Miicam_get_ExpTimeRange(miiHcam, &uimin, &uimax, &uidef);

//     //m_slider_expoTime->setRange(uimin, uimax);
//     Miicam_get_ExpoAGainRange(miiHcam, &usmin, &usmax, &usdef);
//     //m_slider_expoGain->setRange(usmin, usmax);
//     if (0 == (miiDevice.model->flag & MIICAM_FLAG_MONO))
//     {
//         handleTempTintEvent();
//     }
//     handleExpoEvent();

//     if (SUCCEEDED(Miicam_StartPullModeWithCallback(miiHcam, eventCallBack, this)))
//     {
//         //m_cmb_res->setEnabled(true);
//         //m_cbox_auto->setEnabled(true);
//         //m_btn_autoWB->setEnabled(0 == (m_cur.model->flag & MIICAM_FLAG_MONO));
//         //m_slider_temp->setEnabled(0 == (m_cur.model->flag & MIICAM_FLAG_MONO));
//         //m_slider_tint->setEnabled(0 == (m_cur.model->flag & MIICAM_FLAG_MONO));
//         //m_btn_open->setText("Close");
//         //m_btn_snap->setEnabled(true);

//         int bAuto = 0;
//         Miicam_get_AutoExpoEnable(miiHcam, &bAuto);
//         //m_cbox_auto->setChecked(1 == bAuto);

//         timer->start(1000);
//     }
//     else
//     {
//         closeCamera();
//         QMessageBox::warning(this, "Warning", "Failed to start camera.");
//     }
// }

// void MainWindow::openCamera()
// {
//     miiHcam = Miicam_Open(miiDevice.id);
//     if (miiHcam)
//     {
//         Miicam_get_eSize(miiHcam, (unsigned*)&resolutionIndex);
//         imgWidth = miiDevice.model->res[resolutionIndex].width;
//         imgHeight = miiDevice.model->res[resolutionIndex].height;

//         // {
//         //     const QSignalBlocker blocker(m_cmb_res);
//         //     m_cmb_res->clear();
//         //     for (unsigned i = 0; i < m_cur.model->preview; ++i)
//         //         m_cmb_res->addItem(QString::asprintf("%u*%u", m_cur.model->res[i].width, m_cur.model->res[i].height));
//         //     m_cmb_res->setCurrentIndex(m_res);
//         //     m_cmb_res->setEnabled(true);
//         // }

//         Miicam_put_Option(miiHcam, MIICAM_OPTION_BYTEORDER, 0); //Qimage use RGB byte order
//         Miicam_put_AutoExpoEnable(miiHcam, 1);
//         startCamera();
//     }
// }

// void MainWindow::closeCamera()
// {
//     if (miiHcam)
//     {
//         Miicam_Close(miiHcam);
//         miiHcam = nullptr;
//     }
//     delete[] pData;
//     pData = nullptr;

//     // m_btn_open->setText("Open");
//     // m_timer->stop();
//     // m_lbl_frame->clear();
//     // m_cbox_auto->setEnabled(false);
//     // m_slider_expoGain->setEnabled(false);
//     // m_slider_expoTime->setEnabled(false);
//     // m_btn_autoWB->setEnabled(false);
//     // m_slider_temp->setEnabled(false);
//     // m_slider_tint->setEnabled(false);
//     // m_btn_snap->setEnabled(false);
//     // m_cmb_res->setEnabled(false);
//     // m_cmb_res->clear();
// }

// void MainWindow::eventCallBack(unsigned nEvent, void* pCallbackCtx)
// {
//     MainWindow* pThis = reinterpret_cast<MainWindow*>(pCallbackCtx);
//     emit pThis->evtCallback(nEvent);
// }

// void MainWindow::handleImageEvent()
// {
//     unsigned width = 0, height = 0;
//     if (SUCCEEDED(Miicam_PullImage(miiHcam, pData, 24, &width, &height)))
//     {
//         QImage image(pData, width, height, QImage::Format_RGB888);
//         QImage newimage = image.scaled(ui->lbPreview->width(), ui->lbPreview->height(), Qt::KeepAspectRatio, Qt::FastTransformation);
//         ui->lbPreview->setPixmap(QPixmap::fromImage(newimage));
//     }
// }

// void MainWindow::handleExpoEvent()
// {
//     unsigned time = 0;
//     unsigned short gain = 0;
//     Miicam_get_ExpoTime(miiHcam, &time);
//     Miicam_get_ExpoAGain(miiHcam, &gain);

//     // {
//     //     const QSignalBlocker blocker(m_slider_expoTime);
//     //     m_slider_expoTime->setValue(int(time));
//     // }
//     // {
//     //     const QSignalBlocker blocker(m_slider_expoGain);
//     //     m_slider_expoGain->setValue(int(gain));
//     // }
//     // m_lbl_expoTime->setText(QString::number(time));
//     // m_lbl_expoGain->setText(QString::number(gain));
// }

// void MainWindow::handleTempTintEvent()
// {
//     int nTemp = 0, nTint = 0;
//     if (SUCCEEDED(Miicam_get_TempTint(miiHcam, &nTemp, &nTint)))
//     {
//         // {
//         //     const QSignalBlocker blocker(m_slider_temp);
//         //     m_slider_temp->setValue(nTemp);
//         // }
//         // {
//         //     const QSignalBlocker blocker(m_slider_tint);
//         //     m_slider_tint->setValue(nTint);
//         // }
//         // m_lbl_temp->setText(QString::number(nTemp));
//         // m_lbl_tint->setText(QString::number(nTint));
//     }
// }

// void MainWindow::handleStillImageEvent()
// {
//     unsigned width = 0, height = 0;
//     if (SUCCEEDED(Miicam_PullStillImage(miiHcam, nullptr, 24, &width, &height))) // peek
//     {
//         std::vector<uchar> vec(TDIBWIDTHBYTES(width * 24) * height);
//         if (SUCCEEDED(Miicam_PullStillImage(miiHcam, &vec[0], 24, &width, &height)))
//         {
//             QImage image(&vec[0], width, height, QImage::Format_RGB888);
//             image.save(QString::asprintf("demoqt_%u.jpg", ++count));
//         }
//     }
// }


void MainWindow::onClickOpenVideo()
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

void MainWindow::onClickPlayVideo()
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


void MainWindow::onClickStopVideo()
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
        ui->btnPlayVideo->setText("Play");
        isVideoPlay = false;
    }
    else
    {
        ui->btnPlayVideo->setText("Pause");
        isVideoPlay = true;
    }
}


void MainWindow::onClickLoadFrames()
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


// void MainWindow::resizeEvent(QResizeEvent* event)
// {
//     QMainWindow::resizeEvent(event);
//     if (!currentPixmap.isNull()) {
//         ui->lbFrameCapture->setPixmap(currentPixmap.scaled(ui->lbFrameCapture->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
//     }
// }
