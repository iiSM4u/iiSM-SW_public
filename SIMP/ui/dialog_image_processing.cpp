#include "dialog_image_processing.h"
#include "ui_dialog_image_processing.h"

DialogImageProcessing::DialogImageProcessing(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogImageProcessing)
{
    ui->setupUi(this);
}

DialogImageProcessing::~DialogImageProcessing()
{
    delete ui;
}
