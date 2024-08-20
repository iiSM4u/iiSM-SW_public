#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tab_camera.h"
#include "tab_video.h"
#include "tab_frame.h"
#include "simp_gegl.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qApp->setStyleSheet(
        "QPlainTextEdit:disabled { background-color: lightgray; color: darkgray; }"
        "QPushButton:disabled { background-color: lightgray; color: darkgray; }"
        "QComboBox:disabled { background-color: lightgray; color: darkgray; }"
        "QCheckBox:disabled { color: darkgray; }"
        "QSlider:disabled { color: darkgray; }"
        "QRadioButton:disabled { color: darkgray; }"
        "QSpinBox:disabled { background-color: lightgray; color: darkgray; }"
    );

    SimpGEGL::Init(QCoreApplication::applicationDirPath());

    ui->tabWidget->addTab(new TabCamera(this), "Preview");
    ui->tabWidget->addTab(new TabVideo(this), "Video");
    ui->tabWidget->addTab(new TabFrame(this), "Capture");
}

MainWindow::~MainWindow()
{
    delete ui;
}


