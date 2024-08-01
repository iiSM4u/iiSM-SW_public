#pragma once

#include <QDialog>
#include <QJsonArray>
#include "preset_brightness_contrast.h"

namespace Ui {
class dialog_brightness_contrast;
}

class dialog_brightness_contrast : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_brightness_contrast(QWidget *parent = nullptr);
    dialog_brightness_contrast(std::vector<preset_brightness_contrast>& presets, const double brightness, const double contrast, const bool enable, QWidget *parent = nullptr);
    ~dialog_brightness_contrast();

    std::vector<preset_brightness_contrast> getPresets() const;
    double getBrightness() const;
    double getContrast() const;
    bool getEnable() const;

private slots:
    void chkBrightnessContrast_CheckedChanged(Qt::CheckState checkState);
    void cbPreset_SelectedIndexChanged(int index);
    void btnSavePreset_Click();

    void sliderBrightness_sliderMoved(int position);
    void editBrightness_editingFinished();

    void sliderContrast_sliderMoved(int position);
    void editContrast_editingFinished();

    void EnableUI(bool enable);
    void UpdatePresetUI(const std::vector<preset_brightness_contrast>& presets, const int index = -1);

private:
    Ui::dialog_brightness_contrast *ui;

    std::vector<preset_brightness_contrast> presets;
};
