#pragma once

#include <QDialog>
#include "preset_stress.h"

namespace Ui {
class DialogStress;
}

class DialogStress : public QDialog
{
    Q_OBJECT

public:
    explicit DialogStress(QWidget *parent = nullptr);
    DialogStress(const std::vector<PresetStress>& presets, const int radius, const int samples, int const iterations, bool const enhaceShadows, const bool enable, QWidget *parent = nullptr);
    ~DialogStress();

    std::vector<PresetStress> getPresets() const;
    int getRadius() const;
    int getSamples() const;
    int getIterations() const;
    bool getEnhanceShadows() const;
    bool getEnable() const;

private slots:
    void chkStress_CheckedChanged(Qt::CheckState checkState);
    void cbPreset_SelectedIndexChanged(int index);

    void btnRemovePreset_Click();
    void btnSavePreset_Click();
    void btnResetPreset_Click();

    void sliderRadius_sliderMoved(int position);
    void editRadius_editingFinished();

    void sliderSamples_sliderMoved(int position);
    void editSamples_editingFinished();

    void sliderIterations_sliderMoved(int position);
    void editIterations_editingFinished();

    void EnableUI(bool enable);
    void UpdatePresetUI(const std::vector<PresetStress>& presets, const int index = -1);

private:
    Ui::DialogStress *ui;

    std::vector<PresetStress> presets;
};
