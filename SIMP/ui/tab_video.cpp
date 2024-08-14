#include "tab_video.h"
#include "ui_tab_video.h"

TabVideo::TabVideo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TabVideo)
{
    ui->setupUi(this);
}

TabVideo::~TabVideo()
{
    delete ui;
}
