#include "dialog_brightness_contrast.h"
#include "ui_dialog_brightness_contrast.h"
#include "constants.h"
#include "utils.h"

#include <QMessageBox>
#include <QJsonObject>

dialog_brightness_contrast::dialog_brightness_contrast(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialog_brightness_contrast)
{
    ui->setupUi(this);
}

dialog_brightness_contrast::dialog_brightness_contrast(bool enable, double brightness, double contrast, std::vector<preset_brightness_contrast>& presets, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialog_brightness_contrast)
    , presets(presets)
{
    ui->setupUi(this);

    connect(ui->chkBrightnessContrast, &QCheckBox::checkStateChanged, this, &dialog_brightness_contrast::chkBrightnessContrast_CheckedChanged);
    connect(ui->cbPresets, &QComboBox::currentIndexChanged, this, &dialog_brightness_contrast::cbPreset_SelectedIndexChanged);
    connect(ui->btnSavePreset, &QPushButton::clicked, this, &dialog_brightness_contrast::btnSavePreset_Click);

    connect(ui->sliderBrightness, &QSlider::sliderMoved, this, &dialog_brightness_contrast::sliderBrightness_sliderMoved);
    connect(ui->editBrightness, &CustomPlainTextEdit::editingFinished, this, &dialog_brightness_contrast::editBrightness_editingFinished);
    connect(ui->sliderContrast, &QSlider::sliderMoved, this, &dialog_brightness_contrast::sliderContrast_sliderMoved);
    connect(ui->editContrast, &CustomPlainTextEdit::editingFinished, this, &dialog_brightness_contrast::editContrast_editingFinished);

    // combobox를 업데이트하면서 slider가 업데이트 되기 때문에 slider 보다 먼저 combobox를 업데이트한다.
    dialog_brightness_contrast::UpdatePresetUI(this->presets);

    // set min-max
    // 10을 곱한다.
    ui->sliderBrightness->setMinimum((int)(GEGL_BRIGHTNESS_MIN * 10.0));
    ui->sliderBrightness->setMaximum((int)(GEGL_BRIGHTNESS_MAX * 10.0));
    ui->sliderBrightness->setValue((int)(brightness * 10.0));
    ui->editBrightness->setPlainText(QString::number(brightness, 'f', 1));

    ui->sliderContrast->setMinimum((int)(GEGL_CONTRAST_MIN * 10.0));
    ui->sliderContrast->setMaximum((int)(GEGL_CONTRAST_MAX * 10.0));
    ui->sliderContrast->setValue((int)(contrast * 10.0));
    ui->editContrast->setPlainText(QString::number(contrast, 'f', 1));

    ui->chkBrightnessContrast->setCheckState(enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    dialog_brightness_contrast::EnableUI(enable);
}

dialog_brightness_contrast::~dialog_brightness_contrast()
{
    delete ui;
}

bool dialog_brightness_contrast::getEnable() const
{
    return ui->chkBrightnessContrast->isChecked();
}

double dialog_brightness_contrast::getBrightness() const
{
    bool ok;
    double value = ui->editBrightness->toPlainText().toDouble(&ok);
    return value;
}

double dialog_brightness_contrast::getContrast() const
{
    bool ok;
    double value = ui->editContrast->toPlainText().toDouble(&ok);
    return value;
}

void dialog_brightness_contrast::chkBrightnessContrast_CheckedChanged(Qt::CheckState checkState)
{
    dialog_brightness_contrast::EnableUI(checkState == Qt::CheckState::Checked);
}

void dialog_brightness_contrast::cbPreset_SelectedIndexChanged(int index)
{
    if (index > -1)
    {
        preset_brightness_contrast preset = this->presets[index];

        // 10을 곱한다.
        ui->sliderBrightness->setValue((int)(preset.GetBrightness() * 10.0));
        ui->editBrightness->setPlainText(QString::number(preset.GetBrightness(), 'f', 1));

        ui->sliderContrast->setValue((int)(preset.GetContrast() * 10.0));
        ui->editContrast->setPlainText(QString::number(preset.GetContrast(), 'f', 1));
    }
}

void dialog_brightness_contrast::btnSavePreset_Click()
{
    bool ok;
    double brightness = ui->editBrightness->toPlainText().toDouble(&ok);
    double contrast = ui->editContrast->toPlainText().toDouble(&ok);
    int index = this->presets.size();

    this->presets.emplace_back(index, brightness, contrast);

    dialog_brightness_contrast::UpdatePresetUI(this->presets, index);
}

void dialog_brightness_contrast::sliderBrightness_sliderMoved(int position)
{
    // trackbar가 정수이므로 0.1을 곱한다.
    double value = ui->sliderBrightness->value() * 0.1;
    ui->editBrightness->setPlainText(QString::number(value, 'f', 1));
}

void dialog_brightness_contrast::editBrightness_editingFinished()
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
            QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            value = roundToDecimalPlaces(ui->sliderBrightness->value() * 0.1, 1);
            ui->editBrightness->setPlainText(QString::number(value, 'f', 1));
        }
    }
    else
    {
        QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        value = roundToDecimalPlaces(ui->sliderBrightness->value() * 0.1, 1);
        ui->editBrightness->setPlainText(QString::number(value, 'f', 1));
    }
}

void dialog_brightness_contrast::sliderContrast_sliderMoved(int position)
{
    // trackbar가 정수이므로 0.1을 곱한다.
    double value = ui->sliderContrast->value() * 0.1;    
    ui->editContrast->setPlainText(QString::number(value, 'f', 1));
}

void dialog_brightness_contrast::editContrast_editingFinished()
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
            QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            value = roundToDecimalPlaces(ui->sliderContrast->value() * 0.1, 1);
            ui->editContrast->setPlainText(QString::number(value, 'f', 1));
        }
    }
    else
    {
        QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        value = roundToDecimalPlaces(ui->sliderContrast->value() * 0.1, 1);
        ui->editContrast->setPlainText(QString::number(value, 'f', 1));
    }
}

void dialog_brightness_contrast::EnableUI(bool enable)
{
    ui->cbPresets->setEnabled(enable);
    ui->btnSavePreset->setEnabled(enable);
    ui->sliderBrightness->setEnabled(enable);
    ui->editBrightness->setEnabled(enable);
    ui->sliderContrast->setEnabled(enable);
    ui->editContrast->setEnabled(enable);
}

void dialog_brightness_contrast::UpdatePresetUI(const std::vector<preset_brightness_contrast>& presets, const int index)
{
    ui->cbPresets->clear();

    if (presets.size() > 0)
    {
        for (const preset_brightness_contrast& preset : presets)
        {
            QString message = QString("brightness: %1, contrast: %2")
                                  .arg(preset.GetBrightness(), 0, 'f', 1) // 'f' for floating point, 2 decimal places
                                  .arg(preset.GetContrast(), 0, 'f', 1);
            ui->cbPresets->addItem(message);
        }
    }

    ui->cbPresets->setCurrentIndex(index);
}
