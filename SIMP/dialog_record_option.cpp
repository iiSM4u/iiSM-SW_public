#include "dialog_record_option.h"
#include "ui_dialog_record_option.h"

dialog_record_option::dialog_record_option(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialog_record_option)
{
    ui->setupUi(this);
}

dialog_record_option::~dialog_record_option()
{
    delete ui;
}
