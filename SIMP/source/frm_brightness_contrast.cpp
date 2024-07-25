#include "frm_brightness_contrast.h"
#include "ui_frm_brightness_contrast.h"

frm_brightness_contrast::frm_brightness_contrast(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::frm_brightness_contrast)
{
    ui->setupUi(this);
}

frm_brightness_contrast::~frm_brightness_contrast()
{
    delete ui;
}
