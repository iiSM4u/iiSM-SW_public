#pragma once

#include <QDialog>
#include <QJsonArray>
#include "preset_brightness_contrast.h"

namespace Ui {
class DialogBrightnessContrast;
}

class DialogBrightnessContrast : public QDialog
{
    Q_OBJECT

public:
    explicit DialogBrightnessContrast(QWidget *parent = nullptr);
    DialogBrightnessContrast(std::vector<PresetBrightnessContrast>& presets, const double brightness, const double contrast, const bool enable, QWidget *parent = nullptr);
    ~DialogBrightnessContrast();

    std::vector<PresetBrightnessContrast> getPresets() const;
    double getBrightness() const;
    double getContrast() const;
    bool getEnable() const;

private slots:
    void chkBrightnessContrast_CheckedChanged(Qt::CheckState checkState);
    void cbPreset_SelectedIndexChanged(int index);
    void btnSavePreset_Click();
    void btnRemovePreset_Click();
    void btnResetPreset_Click();

    void sliderBrightness_sliderMoved(int position);
    void editBrightness_editingFinished();

    void sliderContrast_sliderMoved(int position);
    void editContrast_editingFinished();

    void EnableUI(bool enable);
    void UpdatePresetUI(const std::vector<PresetBrightnessContrast>& presets, const int index = -1);

private:
    Ui::DialogBrightnessContrast *ui;

    std::vector<PresetBrightnessContrast> presets;
};
