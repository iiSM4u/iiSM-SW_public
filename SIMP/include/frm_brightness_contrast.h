#pragma once

#include <QDialog>

namespace Ui {
class frm_brightness_contrast;
}

class frm_brightness_contrast : public QDialog
{
    Q_OBJECT

public:
    explicit frm_brightness_contrast(QWidget *parent = nullptr);
    ~frm_brightness_contrast();

private slots:
    void chkBrightnessContrast_CheckedChanged(Qt::CheckState checkState);
    void cbPreset_SelectedIndexChanged(int index);
    void btnSavePreset_Click();

    void sliderBrightness_sliderMoved(int position);
    void editBrightness_editingFinished();

    void sliderContrast_sliderMoved(int position);
    void editContrast_editingFinished();

private:
    Ui::frm_brightness_contrast *ui;
};
