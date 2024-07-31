#pragma once

#include <QDialog>
#include "preset_contrast_curve.h"

namespace Ui {
class dialog_contrast_curve;
}

class dialog_contrast_curve : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_contrast_curve(QWidget *parent = nullptr);
    dialog_contrast_curve(bool enable, std::vector<preset_contrast_curve>& presets, QWidget *parent = nullptr);
    ~dialog_contrast_curve();

    bool getEnable() const;
    int getSelectedIndex() const;

private slots:
    void chkCurve_CheckedChanged(Qt::CheckState checkState);
    void cbPreset_SelectedIndexChanged(int index);
    void btnSavePreset_Click();

    void EnableUI(bool enable);
    void UpdatePresetUI(const std::vector<preset_contrast_curve>& presets, const int index = -1);

private:
    Ui::dialog_contrast_curve *ui;

    std::vector<preset_contrast_curve> presets;
};
