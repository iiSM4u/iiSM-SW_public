#include "dialog_record_option.h"
#include "ui_dialog_record_option.h"
#include "simp_const_path.h"
#include "simp_const_value.h"
#include "simp_const_menu.h"

#include <QFileDialog>
#include <QMessageBox>

DialogRecordOption::DialogRecordOption(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogRecordOption)
{
    ui->setupUi(this);
}

DialogRecordOption::DialogRecordOption(const QString& dir, const VideoFormatType format, const int frameRate, const int quality, const int timeLimit, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogRecordOption)
{
    ui->setupUi(this);

    connect(ui->btnDir, &QPushButton::clicked, this, &DialogRecordOption::btnDir_Click);
    connect(ui->chkTimeLimit, &QCheckBox::checkStateChanged, this, &DialogRecordOption::chkTimeLimit_CheckedChanged);
    connect(ui->sliderFrameRate, &QSlider::sliderMoved, this, &DialogRecordOption::sliderFrameRate_sliderMoved);
    connect(ui->editFrameRate, &CustomPlainTextEdit::editingFinished, this, &DialogRecordOption::editFrameRate_editingFinished);

    ui->lbDir->setText(SimpConstPath::DIR_RECORD_VIDEO);

    ui->cbVideoFormat->clear();
    for (const auto& format : {VideoFormatType::MJPEG, VideoFormatType::XVID, VideoFormatType::MP4V, VideoFormatType::NONE })
    {
        ui->cbVideoFormat->addItem(toString(format));
    }
    ui->cbVideoFormat->setCurrentIndex(static_cast<int>(format));

    ui->editQuality->setPlainText(QString::number(quality));

    ui->chkTimeLimit->setChecked(timeLimit > 0);
    ui->editTimeLimit->setPlainText(QString::number(timeLimit));
    ui->editTimeLimit->setEnabled(timeLimit > 0);

    ui->sliderFrameRate->setMinimum(SimpConstValue::RECORD_FRAME_RATE_MIN);
    ui->sliderFrameRate->setMaximum(SimpConstValue::RECORD_FRAME_RATE_MAX);
    ui->sliderFrameRate->setValue(frameRate);
    ui->editFrameRate->setPlainText(QString::number(frameRate));
}

DialogRecordOption::~DialogRecordOption()
{
    delete ui;
}

VideoFormatType DialogRecordOption::getVideoFormat() const
{
    return static_cast<VideoFormatType>(ui->cbVideoFormat->currentIndex());
}

QString DialogRecordOption::getVideoDirectory() const
{
    return ui->lbDir->text();
}

int DialogRecordOption::getQuality() const
{
    bool ok;
    return ui->editQuality->toPlainText().toInt(&ok);
}

int DialogRecordOption::getTimeLimit() const
{
    if (ui->chkTimeLimit->isChecked())
    {
        bool ok;
        return ui->editTimeLimit->toPlainText().toInt(&ok);
    }
    return 0;
}

int DialogRecordOption::getFrameRate() const
{
    return ui->sliderFrameRate->value();
}

void DialogRecordOption::btnDir_Click()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty())
    {
        ui->lbDir->setText(dir);
    }
}

void DialogRecordOption::chkTimeLimit_CheckedChanged(Qt::CheckState checkState)
{
    ui->editTimeLimit->setEnabled(ui->chkTimeLimit->isChecked());
}

void DialogRecordOption::sliderFrameRate_sliderMoved(int position)
{
    int value = ui->sliderFrameRate->value();
    ui->editFrameRate->setPlainText(QString::number(value));
}

void DialogRecordOption::editFrameRate_editingFinished()
{
    bool ok;
    int value = ui->editFrameRate->toPlainText().toInt(&ok);

    if (ok)
    {
        if (value >= ui->sliderFrameRate->minimum() && value <= ui->sliderFrameRate->maximum())
        {
            // slider에도 값 업데이트
            ui->sliderFrameRate->setValue(value);
        }
        else
        {
            QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            ui->editFrameRate->setPlainText(QString::number(ui->sliderFrameRate->value()));
        }
    }
    else
    {
        QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        ui->editFrameRate->setPlainText(QString::number(ui->sliderFrameRate->value()));
    }
}
