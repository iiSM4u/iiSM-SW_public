#include "dialog_brightness_contrast.h"
#include "ui_dialog_brightness_contrast.h"
#include "simp_util.h"
#include "simp_const_value.h"
#include "simp_const_menu.h"

#include <QMessageBox>
#include <QJsonObject>

DialogBrightnessContrast::DialogBrightnessContrast(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogBrightnessContrast)
{
    ui->setupUi(this);
}

DialogBrightnessContrast::DialogBrightnessContrast(std::vector<PresetBrightnessContrast>& presets, const double brightness, const double contrast, const bool enable, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogBrightnessContrast)
    , presets(presets)
{
    ui->setupUi(this);

    connect(ui->chkBrightnessContrast, &QCheckBox::checkStateChanged, this, &DialogBrightnessContrast::chkBrightnessContrast_CheckedChanged);
    connect(ui->cbPresets, &QComboBox::currentIndexChanged, this, &DialogBrightnessContrast::cbPreset_SelectedIndexChanged);

    connect(ui->btnRemovePreset, &QPushButton::clicked, this, &DialogBrightnessContrast::btnRemovePreset_Click);
    connect(ui->btnSavePreset, &QPushButton::clicked, this, &DialogBrightnessContrast::btnSavePreset_Click);
    connect(ui->btnResetPreset, &QPushButton::clicked, this, &DialogBrightnessContrast::btnResetPreset_Click);

    connect(ui->sliderBrightness, &QSlider::sliderMoved, this, &DialogBrightnessContrast::sliderBrightness_sliderMoved);
    connect(ui->editBrightness, &CustomPlainTextEdit::editingFinished, this, &DialogBrightnessContrast::editBrightness_editingFinished);
    connect(ui->sliderContrast, &QSlider::sliderMoved, this, &DialogBrightnessContrast::sliderContrast_sliderMoved);
    connect(ui->editContrast, &CustomPlainTextEdit::editingFinished, this, &DialogBrightnessContrast::editContrast_editingFinished);

    // combobox를 업데이트하면서 slider가 업데이트 되기 때문에 slider 보다 먼저 combobox를 업데이트한다.
    DialogBrightnessContrast::UpdatePresetUI(this->presets);

    // set min-max
    // 10을 곱한다.
    ui->sliderBrightness->setMinimum((int)(SimpConstValue::GEGL_BRIGHTNESS_MIN * 10.0));
    ui->sliderBrightness->setMaximum((int)(SimpConstValue::GEGL_BRIGHTNESS_MAX * 10.0));
    ui->sliderBrightness->setValue((int)(brightness * 10.0));
    ui->editBrightness->setPlainText(QString::number(brightness, 'f', 1));

    ui->sliderContrast->setMinimum((int)(SimpConstValue::GEGL_CONTRAST_MIN * 10.0));
    ui->sliderContrast->setMaximum((int)(SimpConstValue::GEGL_CONTRAST_MAX * 10.0));
    ui->sliderContrast->setValue((int)(contrast * 10.0));
    ui->editContrast->setPlainText(QString::number(contrast, 'f', 1));

    ui->chkBrightnessContrast->setCheckState(enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    DialogBrightnessContrast::EnableUI(enable);
}

DialogBrightnessContrast::~DialogBrightnessContrast()
{
    delete ui;
}

std::vector<PresetBrightnessContrast> DialogBrightnessContrast::getPresets() const
{
    return this->presets;
}

double DialogBrightnessContrast::getBrightness() const
{
    bool ok;
    double value = ui->editBrightness->toPlainText().toDouble(&ok);
    return value;
}

double DialogBrightnessContrast::getContrast() const
{
    bool ok;
    double value = ui->editContrast->toPlainText().toDouble(&ok);
    return value;
}

bool DialogBrightnessContrast::getEnable() const
{
    return ui->chkBrightnessContrast->isChecked();
}

void DialogBrightnessContrast::chkBrightnessContrast_CheckedChanged(Qt::CheckState checkState)
{
    DialogBrightnessContrast::EnableUI(checkState == Qt::CheckState::Checked);
}

void DialogBrightnessContrast::cbPreset_SelectedIndexChanged(int index)
{
    // combobox가 -1일 때는 default 값으로 띄운다.
    double brightness = SimpConstValue::GEGL_BRIGHTNESS_DEFAULT;
    double contrast = SimpConstValue::GEGL_CONTRAST_DEFAULT;

    if (index > -1)
    {
        PresetBrightnessContrast preset = this->presets[index];
        brightness = preset.GetBrightness();
        contrast = preset.GetContrast();
    }

    // 10을 곱한다.
    ui->sliderBrightness->setValue((int)(brightness * 10.0));
    ui->editBrightness->setPlainText(QString::number(brightness, 'f', 1));

    ui->sliderContrast->setValue((int)(contrast * 10.0));
    ui->editContrast->setPlainText(QString::number(contrast, 'f', 1));
}

void DialogBrightnessContrast::btnRemovePreset_Click()
{
    if (ui->cbPresets->currentIndex() > -1)
    {
        this->presets.erase(this->presets.begin() + ui->cbPresets->currentIndex());
        DialogBrightnessContrast::UpdatePresetUI(this->presets);
    }
}

void DialogBrightnessContrast::btnSavePreset_Click()
{
    bool ok;
    double brightness = ui->editBrightness->toPlainText().toDouble(&ok);
    double contrast = ui->editContrast->toPlainText().toDouble(&ok);

    int index = this->presets.size();
    this->presets.emplace_back(index, brightness, contrast);

    DialogBrightnessContrast::UpdatePresetUI(this->presets, index);
}

void DialogBrightnessContrast::btnResetPreset_Click()
{
    ui->cbPresets->setCurrentIndex(-1);
}

void DialogBrightnessContrast::sliderBrightness_sliderMoved(int position)
{
    // trackbar가 정수이므로 0.1을 곱한다.
    double value = ui->sliderBrightness->value() * 0.1;
    ui->editBrightness->setPlainText(QString::number(value, 'f', 1));
}

void DialogBrightnessContrast::editBrightness_editingFinished()
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

void DialogBrightnessContrast::sliderContrast_sliderMoved(int position)
{
    // trackbar가 정수이므로 0.1을 곱한다.
    double value = ui->sliderContrast->value() * 0.1;    
    ui->editContrast->setPlainText(QString::number(value, 'f', 1));
}

void DialogBrightnessContrast::editContrast_editingFinished()
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

void DialogBrightnessContrast::EnableUI(bool enable)
{
    ui->cbPresets->setEnabled(enable);
    ui->btnRemovePreset->setEnabled(enable);
    ui->btnSavePreset->setEnabled(enable);
    ui->btnResetPreset->setEnabled(enable);
    ui->sliderBrightness->setEnabled(enable);
    ui->editBrightness->setEnabled(enable);
    ui->sliderContrast->setEnabled(enable);
    ui->editContrast->setEnabled(enable);
}

void DialogBrightnessContrast::UpdatePresetUI(const std::vector<PresetBrightnessContrast>& presets, const int index)
{
    ui->cbPresets->clear();

    if (presets.size() > 0)
    {
        for (const PresetBrightnessContrast& preset : presets)
        {
            QString message = QString("brightness: %1, contrast: %2")
                                  .arg(preset.GetBrightness(), 0, 'f', 1) // 'f' for floating point, 2 decimal places
                                  .arg(preset.GetContrast(), 0, 'f', 1);
            ui->cbPresets->addItem(message);
        }
    }

    ui->cbPresets->setCurrentIndex(index);
}
