#pragma once

#include <QDialog>
#include "video_format_type.h"

namespace Ui {
class dialog_record_option;
}

class dialog_record_option : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_record_option(QWidget *parent = nullptr);
    dialog_record_option(QString& dir, VideoFormatType format, int frameRate, int quality, int timeLimit, QWidget *parent = nullptr);
    ~dialog_record_option();

    QString getVideoDirectory() const;
    VideoFormatType getVideoFormat() const;
    int getQuality() const;
    int getTimeLimit() const;
    int getFrameRate() const;

private slots:
    void btnDir_Click();
    void chkTimeLimit_CheckedChanged(Qt::CheckState checkState);
    void sliderFrameRate_sliderMoved(int position);
    void editFrameRate_editingFinished();

private:
    Ui::dialog_record_option *ui;
};
