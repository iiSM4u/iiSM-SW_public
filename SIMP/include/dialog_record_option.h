#pragma once

#include <QDialog>
#include "video_format_type.h"
#include "video_encoder_type.h"

namespace Ui {
class dialog_record_option;
}

class dialog_record_option : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_record_option(QWidget *parent = nullptr);
    ~dialog_record_option();

    VideoFormatType getVideoFormat() const;
    QString getVideoDirectory() const;
    VideoEncoderType getVideoEncoder() const;
    int getQuality() const;
    int getTimeLimit() const;
    int getFrameRate() const;

private slots:
    void btnDir_Click();

    void cbVideoFormat_SelectedIndexChanged(int index);
    void cbVideoEncoder_SelectedIndexChanged(int index);
    void editQuality_editingFinished();

    void chkTimeLimit_CheckedChanged(Qt::CheckState checkState);
    void editTimeLimit_editingFinished();

    void sliderFrameRate_sliderMoved(int position);
    void editFrameRate_editingFinished();

private:
    Ui::dialog_record_option *ui;
};
