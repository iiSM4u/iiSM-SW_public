#include "tab_frame.h"
#include "ui_tab_frame.h"

TabFrame::TabFrame(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TabFrame)
{
    ui->setupUi(this);
}

TabFrame::~TabFrame()
{
    delete ui;
}
