#include "dialog_image_processing.h"
#include "ui_dialog_image_processing.h"

dialog_image_processing::dialog_image_processing(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialog_image_processing)
{
    ui->setupUi(this);
}

dialog_image_processing::~dialog_image_processing()
{
    delete ui;
}
