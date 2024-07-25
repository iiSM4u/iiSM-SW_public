#include "dialog_image_curve.h"
#include "ui_dialog_image_curve.h"

dialog_image_curve::dialog_image_curve(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialog_image_curve)
{
    ui->setupUi(this);
}

dialog_image_curve::~dialog_image_curve()
{
    delete ui;
}
