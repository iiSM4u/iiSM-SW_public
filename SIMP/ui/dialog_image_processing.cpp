#include "dialog_image_processing.h"
#include "ui_dialog_image_processing.h"
#include "simp_const_path.h"
#include "simp_const_value.h"
#include "simp_const_menu.h"
#include "simp_util.h"

#include <QMessageBox>

DialogImageProcessing::DialogImageProcessing(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogImageProcessing)
{
    ui->setupUi(this);
}

DialogImageProcessing::DialogImageProcessing(const int selectedIndex, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogImageProcessing)
{
    ui->setupUi(this);

    // ui를 초기화하기 위해 preset부터 load한다.
    QJsonArray jsonArray;
    if (SimpUtil::loadJsonFile(QCoreApplication::applicationDirPath() + SimpConstPath::PATH_JSON_IMAGE_PROCESSING, jsonArray))
    {
        this->presets = SimpUtil::convertJsonToPresetsImageProcessing(jsonArray);
    }

    DialogImageProcessing::ConnectUI();
    DialogImageProcessing::InitUI();

    // default는 false
    DialogImageProcessing::EnableBrightnessContrast(false);
    DialogImageProcessing::EnableStress(false);
    DialogImageProcessing::EnableStretchContrast(false);

    DialogImageProcessing::UpdatePresetCombobox(this->presets, selectedIndex);
}

DialogImageProcessing::~DialogImageProcessing()
{
    delete ui;
}

int DialogImageProcessing::getPresetIndex() const
{
    return ui->cbPresets->currentIndex();
}

bool DialogImageProcessing::getBrightnessContrastEnable() const
{
    return ui->chkBrightnessContrast->isChecked();
}

double DialogImageProcessing::getBrightness() const
{
    bool ok;
    double value = ui->editBrightness->toPlainText().toDouble(&ok);
    return value;
}

double DialogImageProcessing::getContrast() const
{
    bool ok;
    double value = ui->editContrast->toPlainText().toDouble(&ok);
    return value;
}

bool DialogImageProcessing::getStressEnable() const
{
    return ui->chkStress->isChecked();
}

int DialogImageProcessing::getStressRadius() const
{
    return ui->sliderRadius->value();
}

int DialogImageProcessing::getStressSamples() const
{
    return ui->sliderSamples->value();
}

int DialogImageProcessing::getStressIterations() const
{
    return ui->sliderIterations->value();
}

bool DialogImageProcessing::getStressEnhanceShadows() const
{
    return ui->chkEnhanceShadows->isChecked();
}

bool DialogImageProcessing::getStretchContrastEnable() const
{
    return ui->chkStretchContrast->isChecked();
}

bool DialogImageProcessing::getStretchContrastKeepColors() const
{
    return ui->chkKeepColors->isChecked();
}

bool DialogImageProcessing::getStretchContrastNonLinearComponents() const
{
    return ui->chkNonLinearComponents->isChecked();
}

void DialogImageProcessing::ConnectUI()
{
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DialogImageProcessing::onOkClicked);

    connect(ui->cbPresets, &QComboBox::currentIndexChanged, this, &DialogImageProcessing::cbPreset_SelectedIndexChanged);

    connect(ui->btnRemovePreset, &QPushButton::clicked, this, &DialogImageProcessing::btnRemovePreset_Click);
    connect(ui->btnSavePreset, &QPushButton::clicked, this, &DialogImageProcessing::btnSavePreset_Click);
    connect(ui->btnResetPreset, &QPushButton::clicked, this, &DialogImageProcessing::btnResetPreset_Click);

    connect(ui->chkBrightnessContrast, &QCheckBox::checkStateChanged, this, &DialogImageProcessing::chkBrightnessContrast_CheckedChanged);
    connect(ui->chkStress, &QCheckBox::checkStateChanged, this, &DialogImageProcessing::chkStress_CheckedChanged);
    connect(ui->chkStretchContrast, &QCheckBox::checkStateChanged, this, &DialogImageProcessing::chkStretchContrast_CheckedChanged);

    connect(ui->sliderBrightness, &QSlider::sliderMoved, this, &DialogImageProcessing::sliderBrightness_sliderMoved);
    connect(ui->editBrightness, &CustomPlainTextEdit::editingFinished, this, &DialogImageProcessing::editBrightness_editingFinished);
    connect(ui->sliderContrast, &QSlider::sliderMoved, this, &DialogImageProcessing::sliderContrast_sliderMoved);
    connect(ui->editContrast, &CustomPlainTextEdit::editingFinished, this, &DialogImageProcessing::editContrast_editingFinished);

    connect(ui->sliderRadius, &QSlider::sliderMoved, this, &DialogImageProcessing::sliderRadius_sliderMoved);
    connect(ui->editRadius, &CustomPlainTextEdit::editingFinished, this, &DialogImageProcessing::editRadius_editingFinished);
    connect(ui->sliderSamples, &QSlider::sliderMoved, this, &DialogImageProcessing::sliderSamples_sliderMoved);
    connect(ui->editSamples, &CustomPlainTextEdit::editingFinished, this, &DialogImageProcessing::editSamples_editingFinished);
    connect(ui->sliderIterations, &QSlider::sliderMoved, this, &DialogImageProcessing::sliderIterations_sliderMoved);
    connect(ui->editIterations, &CustomPlainTextEdit::editingFinished, this, &DialogImageProcessing::editIterations_editingFinished);

}

void DialogImageProcessing::InitUI()
{
    ui->chkBrightnessContrast->setCheckState(Qt::CheckState::Unchecked);

    ui->sliderBrightness->setMinimum((int)(SimpConstValue::GEGL_BRIGHTNESS_MIN * 10.0));
    ui->sliderBrightness->setMaximum((int)(SimpConstValue::GEGL_BRIGHTNESS_MAX * 10.0));
    ui->sliderBrightness->setValue((int)(SimpConstValue::GEGL_BRIGHTNESS_DEFAULT * 10.0));
    ui->editBrightness->setPlainText(QString::number(SimpConstValue::GEGL_BRIGHTNESS_DEFAULT, 'f', 1));

    ui->sliderContrast->setMinimum((int)(SimpConstValue::GEGL_CONTRAST_MIN * 10.0));
    ui->sliderContrast->setMaximum((int)(SimpConstValue::GEGL_CONTRAST_MAX * 10.0));
    ui->sliderContrast->setValue((int)(SimpConstValue::GEGL_CONTRAST_DEFAULT * 10.0));
    ui->editContrast->setPlainText(QString::number(SimpConstValue::GEGL_CONTRAST_DEFAULT, 'f', 1));

    ui->chkStress->setCheckState(Qt::CheckState::Unchecked);

    ui->sliderRadius->setMinimum(SimpConstValue::GEGL_STRESS_RADIUS_MIN);
    ui->sliderRadius->setMaximum(SimpConstValue::GEGL_STRESS_RADIUS_MAX);
    ui->sliderRadius->setValue(SimpConstValue::GEGL_STRESS_RADIUS_DEFAULT);
    ui->editRadius->setPlainText(QString::number(SimpConstValue::GEGL_STRESS_RADIUS_DEFAULT));

    ui->sliderSamples->setMinimum(SimpConstValue::GEGL_STRESS_SAMPLES_MIN);
    ui->sliderSamples->setMaximum(SimpConstValue::GEGL_STRESS_SAMPLES_MAX);
    ui->sliderSamples->setValue(SimpConstValue::GEGL_STRESS_SAMPLES_DEFAULT);
    ui->editSamples->setPlainText(QString::number(SimpConstValue::GEGL_STRESS_SAMPLES_DEFAULT));

    ui->sliderIterations->setMinimum(SimpConstValue::GEGL_STRESS_ITERATIONS_MIN);
    ui->sliderIterations->setMaximum(SimpConstValue::GEGL_STRESS_ITERATIONS_MAX);
    ui->sliderIterations->setValue(SimpConstValue::GEGL_STRESS_ITERATIONS_DEFAULT);
    ui->editIterations->setPlainText(QString::number(SimpConstValue::GEGL_STRESS_ITERATIONS_DEFAULT));

    ui->chkEnhanceShadows->setCheckState(SimpConstValue::GEGL_STRESS_ENHANCE_SHADOWS_DEFAULT ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

    ui->chkStretchContrast->setCheckState(Qt::CheckState::Unchecked);

    ui->chkKeepColors->setCheckState(SimpConstValue::GEGL_STRETCH_CONTRAST_KEEP_COLORS_DEFAULT ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui->chkNonLinearComponents->setCheckState(SimpConstValue::GEGL_STRETCH_CONTRAST_PERCEPTUAL_DEFAULT ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
}

void DialogImageProcessing::onOkClicked()
{
    if (this->isPresetChanged)
    {
        DialogImageProcessing::UpdateOrInsertPreset(ui->cbPresets->currentIndex());
        DialogImageProcessing::SaveJson(this->presets);
    }

    accept();
}

void DialogImageProcessing::UpdatePresetCombobox(const std::vector<PresetImageProcessing>& presets, const int index)
{
    ui->cbPresets->clear();

    if (presets.size() > 0)
    {
        for (const PresetImageProcessing& preset : presets)
        {
            QString message = QString("index: %1, brightness-contrast: %2, stress: %3, stretch-contrast: %4")
                                  .arg(preset.GetIndex())
                                  .arg(preset.GetBrightnessContrastEnable() ? "Enable" : "Disable")
                                  .arg(preset.GetStressEnable() ? "Enable" : "Disable")
                                  .arg(preset.GetStretchContrastEnable() ? "Enable" : "Disable");

            ui->cbPresets->addItem(message);
        }
    }

    ui->cbPresets->setCurrentIndex(index);
}

void DialogImageProcessing::cbPreset_SelectedIndexChanged(int index)
{
    if (index > -1)
    {
        PresetImageProcessing preset = this->presets[index];

        DialogImageProcessing::UpdateSetting(
            /*brightnessContrastEnable*/preset.GetBrightnessContrastEnable()
            , /*brightness*/preset.GetBrightness()
            , /*contrast*/preset.GetContrast()
            , /*stressEnable*/preset.GetStressEnable()
            , /*radius*/preset.GetStressRadius()
            , /*samples*/preset.GetStressSamples()
            , /*iterations*/preset.GetStressIterations()
            , /*enhanceShadows*/preset.GetStressEnhanceShadows()
            , /*stretchContrastEnable*/preset.GetStretchContrastEnable()
            , /*keepColors*/preset.GetStretchContrastKeepColors()
            , /*nonLinearComponents*/preset.GetStretchContrastNonLinearComponents()
        );
    }
    else
    {
        DialogImageProcessing::UpdateSetting(
            /*brightnessContrastEnable*/false
            , /*brightness*/SimpConstValue::GEGL_BRIGHTNESS_DEFAULT
            , /*contrast*/SimpConstValue::GEGL_CONTRAST_DEFAULT
            , /*stressEnable*/false
            , /*radius*/SimpConstValue::GEGL_STRESS_RADIUS_DEFAULT
            , /*samples*/SimpConstValue::GEGL_STRESS_SAMPLES_DEFAULT
            , /*iterations*/SimpConstValue::GEGL_STRESS_ITERATIONS_DEFAULT
            , /*enhanceShadows*/SimpConstValue::GEGL_STRESS_ENHANCE_SHADOWS_DEFAULT
            , /*stretchContrastEnable*/false
            , /*keepColors*/SimpConstValue::GEGL_STRETCH_CONTRAST_KEEP_COLORS_DEFAULT
            , /*nonLinearComponents*/SimpConstValue::GEGL_STRETCH_CONTRAST_PERCEPTUAL_DEFAULT
        );
    }

    this->isPresetChanged = false;
}

void DialogImageProcessing::btnRemovePreset_Click()
{
    if (ui->cbPresets->currentIndex() > -1)
    {
        this->presets.erase(this->presets.begin() + ui->cbPresets->currentIndex());
        DialogImageProcessing::SaveJson(this->presets);
        this->isPresetChanged = false;

        DialogImageProcessing::UpdatePresetCombobox(this->presets);
    }
}

void DialogImageProcessing::btnSavePreset_Click()
{
    int index = ui->cbPresets->currentIndex();
    DialogImageProcessing::UpdateOrInsertPreset(index);
    DialogImageProcessing::SaveJson(this->presets);
    this->isPresetChanged = false;

    DialogImageProcessing::UpdatePresetCombobox(this->presets, index > -1 ? index : this->presets.size() - 1);
}

void DialogImageProcessing::btnResetPreset_Click()
{
    ui->cbPresets->setCurrentIndex(-1);
}

void DialogImageProcessing::sliderBrightness_sliderMoved(int position)
{
    // trackbar가 정수이므로 0.1을 곱한다.
    double value = ui->sliderBrightness->value() * 0.1;
    ui->editBrightness->setPlainText(QString::number(value, 'f', 1));
}

void DialogImageProcessing::chkBrightnessContrast_CheckedChanged(Qt::CheckState checkState)
{
    DialogImageProcessing::EnableBrightnessContrast(checkState == Qt::CheckState::Checked);
    this->isPresetChanged = true;
}

void DialogImageProcessing::chkStress_CheckedChanged(Qt::CheckState checkState)
{
    DialogImageProcessing::EnableStress(checkState == Qt::CheckState::Checked);
    this->isPresetChanged = true;
}

void DialogImageProcessing::chkStretchContrast_CheckedChanged(Qt::CheckState checkState)
{
    DialogImageProcessing::EnableStretchContrast(checkState == Qt::CheckState::Checked);
    this->isPresetChanged = true;
}

void DialogImageProcessing::editBrightness_editingFinished()
{
    bool ok;
    double value = ui->editBrightness->toPlainText().toDouble(&ok);

    if (ok)
    {
        // trackbar에는 정수로 들어가야 하므로 10을 곱한다.
        int valueInt = (int)(value * 10.0);

        if (valueInt >= ui->sliderBrightness->minimum() && valueInt <= ui->sliderBrightness->maximum())
        {
            // slider에도 값 업데이트
            ui->sliderBrightness->setValue(valueInt);
            this->isPresetChanged = true;
        }
        else
        {
            QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            value = SimpUtil::roundToDecimalPlaces(ui->sliderBrightness->value() * 0.1, 1);
            ui->editBrightness->setPlainText(QString::number(value, 'f', 1));
        }
    }
    else
    {
        QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        value = SimpUtil::roundToDecimalPlaces(ui->sliderBrightness->value() * 0.1, 1);
        ui->editBrightness->setPlainText(QString::number(value, 'f', 1));
    }
}

void DialogImageProcessing::sliderContrast_sliderMoved(int position)
{
    // trackbar가 정수이므로 0.1을 곱한다.
    double value = ui->sliderContrast->value() * 0.1;
    ui->editContrast->setPlainText(QString::number(value, 'f', 1));
    this->isPresetChanged = true;
}

void DialogImageProcessing::editContrast_editingFinished()
{
    bool ok;
    double value = ui->editContrast->toPlainText().toDouble(&ok);

    if (ok)
    {
        // trackbar에는 정수로 들어가야 하므로 10을 곱한다.
        int valueInt = (int)(value * 10.0);

        if (valueInt >= ui->sliderContrast->minimum() && valueInt <= ui->sliderContrast->maximum())
        {
            // slider에도 값 업데이트
            ui->sliderContrast->setValue(valueInt);
            this->isPresetChanged = true;
        }
        else
        {
            QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            value = SimpUtil::roundToDecimalPlaces(ui->sliderContrast->value() * 0.1, 1);
            ui->editContrast->setPlainText(QString::number(value, 'f', 1));
        }
    }
    else
    {
        QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        value = SimpUtil::roundToDecimalPlaces(ui->sliderContrast->value() * 0.1, 1);
        ui->editContrast->setPlainText(QString::number(value, 'f', 1));
    }
}

void DialogImageProcessing::sliderRadius_sliderMoved(int position)
{
    int value = ui->sliderRadius->value();
    ui->editRadius->setPlainText(QString::number(value));
    this->isPresetChanged = true;
}

void DialogImageProcessing::editRadius_editingFinished()
{
    bool ok;
    int value = ui->editRadius->toPlainText().toInt(&ok);

    if (ok)
    {
        if (value >= ui->sliderRadius->minimum() && value <= ui->sliderRadius->maximum())
        {
            // slider에도 값 업데이트
            ui->sliderRadius->setValue(value);
            this->isPresetChanged = true;
        }
        else
        {
            QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            ui->editRadius->setPlainText(QString::number(ui->sliderRadius->value()));
        }
    }
    else
    {
        QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        ui->editRadius->setPlainText(QString::number(ui->sliderRadius->value()));
    }
}

void DialogImageProcessing::sliderSamples_sliderMoved(int position)
{
    int value = ui->sliderSamples->value();
    ui->editSamples->setPlainText(QString::number(value));
    this->isPresetChanged = true;
}

void DialogImageProcessing::editSamples_editingFinished()
{
    bool ok;
    int value = ui->editSamples->toPlainText().toInt(&ok);

    if (ok)
    {
        if (value >= ui->sliderSamples->minimum() && value <= ui->sliderSamples->maximum())
        {
            // slider에도 값 업데이트
            ui->sliderSamples->setValue(value);
            this->isPresetChanged = true;
        }
        else
        {
            QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            ui->editSamples->setPlainText(QString::number(ui->sliderSamples->value()));
        }
    }
    else
    {
        QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        ui->editSamples->setPlainText(QString::number(ui->sliderSamples->value()));
    }
}


void DialogImageProcessing::sliderIterations_sliderMoved(int position)
{
    int value = ui->sliderIterations->value();
    ui->editIterations->setPlainText(QString::number(value));
    this->isPresetChanged = true;
}

void DialogImageProcessing::editIterations_editingFinished()
{
    bool ok;
    int value = ui->editIterations->toPlainText().toInt(&ok);

    if (ok)
    {
        if (value >= ui->sliderIterations->minimum() && value <= ui->sliderIterations->maximum())
        {
            // slider에도 값 업데이트
            ui->sliderIterations->setValue(value);
            this->isPresetChanged = true;
        }
        else
        {
            QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            ui->editIterations->setPlainText(QString::number(ui->sliderIterations->value()));
        }
    }
    else
    {
        QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        ui->editIterations->setPlainText(QString::number(ui->sliderIterations->value()));
    }
}

void DialogImageProcessing::EnableBrightnessContrast(bool enable)
{
    ui->sliderBrightness->setEnabled(enable);
    ui->editBrightness->setEnabled(enable);
    ui->sliderContrast->setEnabled(enable);
    ui->editContrast->setEnabled(enable);
}

void DialogImageProcessing::EnableStress(bool enable)
{
    ui->sliderRadius->setEnabled(enable);
    ui->editRadius->setEnabled(enable);
    ui->sliderSamples->setEnabled(enable);
    ui->editSamples->setEnabled(enable);
    ui->sliderIterations->setEnabled(enable);
    ui->editIterations->setEnabled(enable);
    ui->chkEnhanceShadows->setEnabled(enable);
}

void DialogImageProcessing::EnableStretchContrast(bool enable)
{
    ui->chkKeepColors->setEnabled(enable);
    ui->chkNonLinearComponents->setEnabled(enable);
}

void DialogImageProcessing::UpdateSetting(
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
)
{
    ui->chkBrightnessContrast->setCheckState(brightnessContrastEnable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui->sliderBrightness->setValue((int)(brightness * 10.0));
    ui->editBrightness->setPlainText(QString::number(brightness, 'f', 1));

    ui->sliderContrast->setValue((int)(contrast * 10.0));
    ui->editContrast->setPlainText(QString::number(contrast, 'f', 1));

    ui->chkStress->setCheckState(stressEnable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui->sliderRadius->setValue(radius);
    ui->editRadius->setPlainText(QString::number(radius));
    ui->sliderSamples->setValue(samples);
    ui->editSamples->setPlainText(QString::number(samples));
    ui->sliderIterations->setValue(iterations);
    ui->editIterations->setPlainText(QString::number(iterations));
    ui->chkEnhanceShadows->setCheckState(enhanceShadows ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

    ui->chkStretchContrast->setCheckState(stretchContrastEnable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui->chkKeepColors->setCheckState(keepColors ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui->chkNonLinearComponents->setCheckState(nonLinearComponents ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
}

void DialogImageProcessing::UpdateOrInsertPreset(const int selectedIndex)
{
    bool ok;

    // 현재 combobox에 선택된 index가 있었으면 update
    if (selectedIndex > -1)
    {
        // combobox의 index와 preset의 index는 다르다.
        int index = this->presets[selectedIndex].GetIndex();
        this->presets[selectedIndex] = PresetImageProcessing(
            /*index*/index
            , /*brightnessContrastEnable*/ui->chkBrightnessContrast->isChecked()
            , /*brightness*/ui->editBrightness->toPlainText().toDouble(&ok)
            , /*contrast*/ui->editContrast->toPlainText().toDouble(&ok)
            , /*stressEnable*/ui->chkStress->isChecked()
            , /*radius*/ui->sliderRadius->value()
            , /*samples*/ui->sliderSamples->value()
            , /*iterations*/ui->sliderIterations->value()
            , /*enhanceShadows*/ui->chkEnhanceShadows->isChecked()
            , /*stretchContrastEnable*/ui->chkStretchContrast->isChecked()
            , /*keepColors*/ui->chkKeepColors->isChecked()
            , /*nonLinearComponents*/ui->chkNonLinearComponents->isChecked()
        );
    }
    // combobox가 -1이었으면 add
    else
    {
        int index = this->presets.size() > 0 ? this->presets[this->presets.size() - 1].GetIndex() + 1 : 0;
        this->presets.emplace_back(
            /*index*/index
            , /*brightnessContrastEnable*/ui->chkBrightnessContrast->isChecked()
            , /*brightness*/ui->editBrightness->toPlainText().toDouble(&ok)
            , /*contrast*/ui->editContrast->toPlainText().toDouble(&ok)
            , /*stressEnable*/ui->chkStress->isChecked()
            , /*radius*/ui->sliderRadius->value()
            , /*samples*/ui->sliderSamples->value()
            , /*iterations*/ui->sliderIterations->value()
            , /*enhanceShadows*/ui->chkEnhanceShadows->isChecked()
            , /*stretchContrastEnable*/ui->chkStretchContrast->isChecked()
            , /*keepColors*/ui->chkKeepColors->isChecked()
            , /*nonLinearComponents*/ui->chkNonLinearComponents->isChecked()
        );
    }
}

void DialogImageProcessing::SaveJson(const std::vector<PresetImageProcessing>& presets)
{
    // ok 버튼 눌리면 종료 전에 file로 preset 저장.
    QJsonArray jsonArray;
    SimpUtil::convertPresetsImageProcessingToJsonArray(presets, jsonArray);

    QString pathPreset = QCoreApplication::applicationDirPath() + SimpConstPath::PATH_JSON_IMAGE_PROCESSING;
    SimpUtil::saveJsonFile(pathPreset, jsonArray);
}
