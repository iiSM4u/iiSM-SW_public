#include "dialog_record_option.h"
#include "ui_dialog_record_option.h"

dialog_record_option::dialog_record_option(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialog_record_option)
{
    ui->setupUi(this);

    connect(ui->btnDir, &QPushButton::clicked, this, &dialog_record_option::btnDir_Click);

    connect(ui->cbVideoFormat, &QComboBox::currentIndexChanged, this, &dialog_record_option::cbVideoFormat_SelectedIndexChanged);
    connect(ui->cbVideoEncoder, &QComboBox::currentIndexChanged, this, &dialog_record_option::cbVideoEncoder_SelectedIndexChanged);

    connect(ui->editQuality, &CustomPlainTextEdit::editingFinished, this, &dialog_record_option::editQuality_editingFinished);

    connect(ui->chkTimeLimit, &QCheckBox::checkStateChanged, this, &dialog_record_option::chkTimeLimit_CheckedChanged);
    connect(ui->editTimeLimit, &CustomPlainTextEdit::editingFinished, this, &dialog_record_option::editTimeLimit_editingFinished);

    connect(ui->sliderFrameRate, &QSlider::sliderMoved, this, &dialog_record_option::sliderFrameRate_sliderMoved);
    connect(ui->editFrameRate, &CustomPlainTextEdit::editingFinished, this, &dialog_record_option::editFrameRate_editingFinished);
}

dialog_record_option::~dialog_record_option()
{
    delete ui;
}

VideoFormatType dialog_record_option::getVideoFormat() const
{

}

QString dialog_record_option::getVideoDirectory() const
{

}

VideoEncoderType dialog_record_option::getVideoEncoder() const
{

}

int dialog_record_option::getQuality() const
{
    bool ok;
    return ui->editQuality->toPlainText().toInt(&ok);
}

int dialog_record_option::getTimeLimit() const
{
    bool ok;
    return ui->editTimeLimit->toPlainText().toInt(&ok);
}

int dialog_record_option::getFrameRate() const
{
    //return ui->sliderFrameRate->value();
}

void dialog_record_option::btnDir_Click()
{

}

void dialog_record_option::cbVideoFormat_SelectedIndexChanged(int index)
{

}

void dialog_record_option::cbVideoEncoder_SelectedIndexChanged(int index)
{

}

void dialog_record_option::editQuality_editingFinished()
{

}

void dialog_record_option::chkTimeLimit_CheckedChanged(Qt::CheckState checkState)
{

}

void dialog_record_option::editTimeLimit_editingFinished()
{

}

void dialog_record_option::sliderFrameRate_sliderMoved(int position)
{

}

void dialog_record_option::editFrameRate_editingFinished()
{

}
