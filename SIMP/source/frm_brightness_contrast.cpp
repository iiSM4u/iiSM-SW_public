#include "frm_brightness_contrast.h"
#include "ui/ui_frm_brightness_contrast.h"
#include "constants.h"

#include <QMessageBox>

frm_brightness_contrast::frm_brightness_contrast(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::frm_brightness_contrast)
{
    ui->setupUi(this);

    connect(ui->chkBrightnessContrast, &QCheckBox::checkStateChanged, this, &frm_brightness_contrast::chkBrightnessContrast_CheckedChanged);
    connect(ui->cbPreset, &QComboBox::currentIndexChanged, this, &frm_brightness_contrast::cbPreset_SelectedIndexChanged);
    connect(ui->btnSavePreset, &QPushButton::clicked, this, &frm_brightness_contrast::btnSavePreset_Click);

    connect(ui->sliderBrightness, &QSlider::sliderMoved, this, &frm_brightness_contrast::sliderBrightness_sliderMoved);
    connect(ui->editBrightness, &CustomPlainTextEdit::editingFinished, this, &frm_brightness_contrast::editBrightness_editingFinished);
    connect(ui->sliderContrast, &QSlider::sliderMoved, this, &frm_brightness_contrast::sliderContrast_sliderMoved);
    connect(ui->editContrast, &CustomPlainTextEdit::editingFinished, this, &frm_brightness_contrast::editContrast_editingFinished);

    // load preset
}

frm_brightness_contrast::~frm_brightness_contrast()
{
    delete ui;
}

void frm_brightness_contrast::chkBrightnessContrast_CheckedChanged(Qt::CheckState checkState)
{
    if (checkState == Qt::CheckState::Checked)
    {
        // enable ui
    }
    else if (checkState == Qt::CheckState::Unchecked)
    {
    }
}

void frm_brightness_contrast::cbPreset_SelectedIndexChanged(int index)
{
    // update value
}

void frm_brightness_contrast::btnSavePreset_Click()
{
    // save value
}


void frm_brightness_contrast::sliderBrightness_sliderMoved(int position)
{
    int value = ui->sliderContrast->value();

    // Miicam_put_Contrast(this->miiHcam, value);

    // {
    //     const QSignalBlocker blocker(ui->editContrast);
    //     ui->editContrast->setPlainText(QString::number(round(value)));
    // }
}

void frm_brightness_contrast::editBrightness_editingFinished()
{
    bool ok;
    int value = ui->editContrast->toPlainText().toInt(&ok);

    if (ok)
    {
        if (value >= ui->sliderContrast->minimum() && value <= ui->sliderContrast->maximum())
        {
            // slider에도 값 업데이트
            ui->sliderContrast->setValue(value);
        }
        else
        {
            QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            ui->editContrast->setPlainText(QString::number(ui->sliderContrast->value()));
        }
    }
    else
    {
        QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        ui->editContrast->setPlainText(QString::number(ui->sliderContrast->value()));
    }
}


void frm_brightness_contrast::sliderContrast_sliderMoved(int position)
{
    int value = ui->sliderContrast->value();

    // Miicam_put_Contrast(this->miiHcam, value);

    {
        const QSignalBlocker blocker(ui->editContrast);
        ui->editContrast->setPlainText(QString::number(round(value)));
    }
}

void frm_brightness_contrast::editContrast_editingFinished()
{
    bool ok;
    int value = ui->editContrast->toPlainText().toInt(&ok);

    if (ok)
    {
        if (value >= ui->sliderContrast->minimum() && value <= ui->sliderContrast->maximum())
        {
            // slider에도 값 업데이트
            ui->sliderContrast->setValue(value);
        }
        else
        {
            QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            ui->editContrast->setPlainText(QString::number(ui->sliderContrast->value()));
        }
    }
    else
    {
        QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        ui->editContrast->setPlainText(QString::number(ui->sliderContrast->value()));
    }
}
