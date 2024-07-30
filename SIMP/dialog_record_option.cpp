#include "dialog_record_option.h"
#include "ui_dialog_record_option.h"

#include "constants.h"
#include <QFileDialog>
#include <QMessageBox>

dialog_record_option::dialog_record_option(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialog_record_option)
{
    ui->setupUi(this);    

    dialog_record_option::ConnectUI();
    dialog_record_option::InitUI();
}

dialog_record_option::~dialog_record_option()
{
    delete ui;
}

void dialog_record_option::ConnectUI()
{
    connect(ui->btnDir, &QPushButton::clicked, this, &dialog_record_option::btnDir_Click);
    connect(ui->chkTimeLimit, &QCheckBox::checkStateChanged, this, &dialog_record_option::chkTimeLimit_CheckedChanged);
    connect(ui->sliderFrameRate, &QSlider::sliderMoved, this, &dialog_record_option::sliderFrameRate_sliderMoved);
    connect(ui->editFrameRate, &CustomPlainTextEdit::editingFinished, this, &dialog_record_option::editFrameRate_editingFinished);
}

void dialog_record_option::InitUI()
{
    ui->lbDir->setText(DIR_RECORD_VIDEO);
    ui->editQuality->setPlainText(QString::number(RECORD_QUALITY_DEFAULT));

    ui->cbVideoFormat->clear();
    for (const auto& format : {VideoFormatType::MJPEG, VideoFormatType::XVID, VideoFormatType::MP4V, VideoFormatType::NONE })
    {
        ui->cbVideoFormat->addItem(toString(format));
    }
    ui->cbVideoFormat->setCurrentIndex(0);

    ui->sliderFrameRate->setMinimum(RECORD_FRAME_RATE_MIN);
    ui->sliderFrameRate->setMaximum(RECORD_FRAME_RATE_MAX);
    ui->sliderFrameRate->setValue(RECORD_FRAME_RATE_DEFAULT);
    ui->editFrameRate->setPlainText(QString::number(RECORD_FRAME_RATE_DEFAULT));
}

VideoFormatType dialog_record_option::getVideoFormat() const
{
    return static_cast<VideoFormatType>(ui->cbVideoFormat->currentIndex());
}

QString dialog_record_option::getVideoDirectory() const
{
    return ui->lbDir->text();
}

int dialog_record_option::getQuality() const
{
    bool ok;
    return ui->editQuality->toPlainText().toInt(&ok);
}

int dialog_record_option::getTimeLimit() const
{
    if (ui->chkTimeLimit->isChecked())
    {
        bool ok;
        return ui->editTimeLimit->toPlainText().toInt(&ok);
    }
    return -1;
}

int dialog_record_option::getFrameRate() const
{
    return ui->sliderFrameRate->value();
}

void dialog_record_option::btnDir_Click()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty())
    {
        ui->lbDir->setText(dir);
    }
}

void dialog_record_option::chkTimeLimit_CheckedChanged(Qt::CheckState checkState)
{
    ui->editTimeLimit->setEnabled(ui->chkTimeLimit->isChecked());
}

void dialog_record_option::sliderFrameRate_sliderMoved(int position)
{
    int value = ui->sliderFrameRate->value();
    ui->editFrameRate->setPlainText(QString::number(value));
}

void dialog_record_option::editFrameRate_editingFinished()
{
    bool ok;
    int value = ui->editFrameRate->toPlainText().toInt(&ok);

    if (ok)
    {
        if (value >= ui->sliderFrameRate->minimum() && value <= ui->sliderFrameRate->maximum())        {

            // slider에도 값 업데이트
            ui->sliderFrameRate->setValue(value);
        }
        else
        {
            QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            ui->editFrameRate->setPlainText(QString::number(ui->sliderFrameRate->value()));
        }
    }
    else
    {
        QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        ui->editFrameRate->setPlainText(QString::number(ui->sliderFrameRate->value()));
    }
}
