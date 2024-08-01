#pragma once

#include <QDialog>
#include "preset_stress.h"

namespace Ui {
class dialog_stress;
}

class dialog_stress : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_stress(QWidget *parent = nullptr);
    dialog_stress(const std::vector<preset_stress>& presets, const int radius, const int samples, int const iterations, bool const enhaceShadows, const bool enable, QWidget *parent = nullptr);
    ~dialog_stress();

    std::vector<preset_stress> getPresets() const;
    int getRadius() const;
    int getSamples() const;
    int getIterations() const;
    bool getEnhanceShadows() const;
    bool getEnable() const;

private slots:
    void chkStress_CheckedChanged(Qt::CheckState checkState);
    void cbPreset_SelectedIndexChanged(int index);
    void btnSavePreset_Click();

    void sliderRadius_sliderMoved(int position);
    void editRadius_editingFinished();

    void sliderSamples_sliderMoved(int position);
    void editSamples_editingFinished();

    void sliderIterations_sliderMoved(int position);
    void editIterations_editingFinished();

    void EnableUI(bool enable);
    void UpdatePresetUI(const std::vector<preset_stress>& presets, const int index = -1);

private:
    Ui::dialog_stress *ui;

    std::vector<preset_stress> presets;
};
