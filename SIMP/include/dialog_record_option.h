#pragma once

#include <QDialog>
#include "video_format_type.h"

namespace Ui {
class DialogRecordOption;
}

class DialogRecordOption : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRecordOption(QWidget *parent = nullptr);
    DialogRecordOption(const QString& dir, const VideoFormatType format, const int frameRate, const int quality, const int timeLimit, QWidget *parent = nullptr);
    ~DialogRecordOption();

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
    Ui::DialogRecordOption *ui;
};
