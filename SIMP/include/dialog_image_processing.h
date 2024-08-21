#pragma once

#include <QDialog>
#include "preset_image_processing.h"

namespace Ui {
class DialogImageProcessing;
}

class DialogImageProcessing : public QDialog
{
    Q_OBJECT

public:
    explicit DialogImageProcessing(QWidget *parent = nullptr);    
    DialogImageProcessing(const int selectedIndex, QWidget *parent = nullptr);
    ~DialogImageProcessing();

    int getPresetIndex() const;

    bool getBrightnessContrastEnable() const;
    double getBrightness() const;
    double getContrast() const;

    bool getStressEnable() const;
    int getStressRadius() const;
    int getStressSamples() const;
    int getStressIterations() const;
    bool getStressEnhanceShadows() const;

    bool getStretchContrastEnable() const;
    bool getStretchContrastKeepColors() const;
    bool getStretchContrastNonLinearComponents() const;

signals:
    void applyClicked();  // Apply 버튼 클릭 시 시그널 발생

private slots:
    void onOkClicked();

    void cbPreset_SelectedIndexChanged(int index);
    void btnSavePreset_Click();
    void btnRemovePreset_Click();
    void btnResetPreset_Click();

    void chkBrightnessContrast_CheckedChanged(Qt::CheckState checkState);
    void chkStress_CheckedChanged(Qt::CheckState checkState);
    void chkStretchContrast_CheckedChanged(Qt::CheckState checkState);

    void sliderBrightness_sliderMoved(int position);
    void editBrightness_editingFinished();

    void sliderContrast_sliderMoved(int position);
    void editContrast_editingFinished();

    void sliderRadius_sliderMoved(int position);
    void editRadius_editingFinished();

    void sliderSamples_sliderMoved(int position);
    void editSamples_editingFinished();

    void sliderIterations_sliderMoved(int position);
    void editIterations_editingFinished();

    void UpdatePresetCombobox(const std::vector<PresetImageProcessing>& presets, const int index = -1);

    void EnableBrightnessContrast(bool enable);
    void EnableStress(bool enable);
    void EnableStretchContrast(bool enable);

    void UpdateSetting(
        bool brightnessContrastEnable
        , double brightness
        , double contrast
        , bool stressEnable
        , int radius
        , int samples
        , int iterations
        , bool enhanceShadows
        , bool stretchContrastEnable
        , bool keepColors
        , bool nonLinearComponents
    );

private:
    Ui::DialogImageProcessing *ui;
    QPushButton *applyButton;  // Apply 버튼

    bool isPresetChanged = false;
    std::vector<PresetImageProcessing> presets;

    void ConnectUI();
    void InitUI();

    void UpdateOrInsertPreset(const int selectedIndex);
    void SaveJson(const std::vector<PresetImageProcessing>& presets);
};
