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
    dialog_stress(bool enable, int radius, int samples, int iterations, bool enhaceShadows, QWidget *parent = nullptr);
    ~dialog_stress();

    bool getEnable() const;
    int getRadius() const;
    int getSamples() const;
    int getIterations() const;
    bool getEnhanceShadows() const;

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
    void UpdatePresetUI(const std::vector<preset_stress>& presets);

private:
    Ui::dialog_stress *ui;

    std::vector<preset_stress> presets;

    std::vector<preset_stress> parseJsonArray(const QJsonArray& jsonArray);
    void convertJsonArray(const std::vector<preset_stress>& presets, QJsonArray& jsonArray);
};
