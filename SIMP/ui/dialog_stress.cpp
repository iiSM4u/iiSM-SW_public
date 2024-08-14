#include "dialog_stress.h"
#include "ui_dialog_stress.h"
#include "simp_const_value.h"
#include "simp_const_menu.h"

#include <QMessageBox>
#include <QJsonObject>

DialogStress::DialogStress(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogStress)
{
    ui->setupUi(this);
}

DialogStress::DialogStress(const std::vector<PresetStress>& presets, const int radius, const int samples, int const iterations, bool const enhaceShadows, const bool enable, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogStress)
    , presets(presets)
{
    ui->setupUi(this);

    connect(ui->chkStress, &QCheckBox::checkStateChanged, this, &DialogStress::chkStress_CheckedChanged);
    connect(ui->cbPresets, &QComboBox::currentIndexChanged, this, &DialogStress::cbPreset_SelectedIndexChanged);

    connect(ui->btnRemovePreset, &QPushButton::clicked, this, &DialogStress::btnRemovePreset_Click);
    connect(ui->btnSavePreset, &QPushButton::clicked, this, &DialogStress::btnSavePreset_Click);
    connect(ui->btnResetPreset, &QPushButton::clicked, this, &DialogStress::btnResetPreset_Click);

    connect(ui->sliderRadius, &QSlider::sliderMoved, this, &DialogStress::sliderRadius_sliderMoved);
    connect(ui->editRadius, &CustomPlainTextEdit::editingFinished, this, &DialogStress::editRadius_editingFinished);
    connect(ui->sliderSamples, &QSlider::sliderMoved, this, &DialogStress::sliderSamples_sliderMoved);
    connect(ui->editSamples, &CustomPlainTextEdit::editingFinished, this, &DialogStress::editSamples_editingFinished);
    connect(ui->sliderIterations, &QSlider::sliderMoved, this, &DialogStress::sliderIterations_sliderMoved);
    connect(ui->editIterations, &CustomPlainTextEdit::editingFinished, this, &DialogStress::editIterations_editingFinished);

    // combobox를 업데이트하면서 slider가 업데이트 되기 때문에 slider 보다 먼저 combobox를 업데이트한다.
    DialogStress::UpdatePresetUI(this->presets);

    // set min-max
    ui->sliderRadius->setMinimum(SimpConstValue::GEGL_STRESS_RADIUS_MIN);
    ui->sliderRadius->setMaximum(SimpConstValue::GEGL_STRESS_RADIUS_MAX);
    ui->sliderRadius->setValue(radius);
    ui->editRadius->setPlainText(QString::number(radius));

    ui->sliderSamples->setMinimum(SimpConstValue::GEGL_STRESS_SAMPLES_MIN);
    ui->sliderSamples->setMaximum(SimpConstValue::GEGL_STRESS_SAMPLES_MAX);
    ui->sliderSamples->setValue(samples);
    ui->editSamples->setPlainText(QString::number(samples));

    ui->sliderIterations->setMinimum(SimpConstValue::GEGL_STRESS_ITERATIONS_MIN);
    ui->sliderIterations->setMaximum(SimpConstValue::GEGL_STRESS_ITERATIONS_MAX);
    ui->sliderIterations->setValue(iterations);
    ui->editIterations->setPlainText(QString::number(iterations));

    ui->chkEnhanceShadows->setCheckState(enhaceShadows ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

    ui->chkStress->setCheckState(enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    DialogStress::EnableUI(enable);
}

DialogStress::~DialogStress()
{
    delete ui;
}

std::vector<PresetStress> DialogStress::getPresets() const
{
    return this->presets;
}

int DialogStress::getRadius() const
{
    return ui->sliderRadius->value();
}

int DialogStress::getSamples() const
{
    return ui->sliderSamples->value();
}

int DialogStress::getIterations() const
{
    return ui->sliderIterations->value();
}

bool DialogStress::getEnhanceShadows() const
{
    return ui->chkEnhanceShadows->isChecked();
}

bool DialogStress::getEnable() const
{
    return ui->chkStress->isChecked();
}

void DialogStress::chkStress_CheckedChanged(Qt::CheckState checkState)
{
    DialogStress::EnableUI(checkState == Qt::CheckState::Checked);
}

void DialogStress::cbPreset_SelectedIndexChanged(int index)
{
    int radius = SimpConstValue::GEGL_STRESS_RADIUS_DEFAULT;
    int samples = SimpConstValue::GEGL_STRESS_SAMPLES_DEFAULT;
    int iterations = SimpConstValue::GEGL_STRESS_ITERATIONS_DEFAULT;
    bool enhanceShadows = false;

    if (index > -1)
    {
        PresetStress preset = this->presets[index];

        radius = preset.GetRadius();
        samples = preset.GetSamples();
        iterations = preset.GetIterations();
        enhanceShadows = preset.GetEnhanceShadows();
    }    

    ui->sliderRadius->setValue(radius);
    ui->editRadius->setPlainText(QString::number(radius));

    ui->sliderSamples->setValue(samples);
    ui->editSamples->setPlainText(QString::number(samples));

    ui->sliderIterations->setValue(iterations);
    ui->editIterations->setPlainText(QString::number(iterations));

    ui->chkEnhanceShadows->setCheckState(enhanceShadows ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
}

void DialogStress::btnRemovePreset_Click()
{
    if (ui->cbPresets->currentIndex() > -1)
    {
        this->presets.erase(this->presets.begin() + ui->cbPresets->currentIndex());
        DialogStress::UpdatePresetUI(this->presets);
    }
}

void DialogStress::btnSavePreset_Click()
{
    bool ok;
    int radius = ui->editRadius->toPlainText().toInt(&ok);
    int samples = ui->editSamples->toPlainText().toInt(&ok);
    int iterations = ui->editIterations->toPlainText().toInt(&ok);
    bool enhanceShadows = ui->chkEnhanceShadows->isChecked();

    int index = this->presets.size();
    this->presets.emplace_back(index, radius, samples, iterations, enhanceShadows);

    // 추가한 것으로 선택
    DialogStress::UpdatePresetUI(this->presets, index);
}

void DialogStress::btnResetPreset_Click()
{
    ui->cbPresets->setCurrentIndex(-1);
}

void DialogStress::sliderRadius_sliderMoved(int position)
{
    int value = ui->sliderRadius->value();
    ui->editRadius->setPlainText(QString::number(value));
}

void DialogStress::editRadius_editingFinished()
{
    bool ok;
    int value = ui->editRadius->toPlainText().toInt(&ok);

    if (ok)
    {
        if (value >= ui->sliderRadius->minimum() && value <= ui->sliderRadius->maximum())
        {
            // slider에도 값 업데이트
            ui->sliderRadius->setValue(value);
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

void DialogStress::sliderSamples_sliderMoved(int position)
{
    int value = ui->sliderSamples->value();
    ui->editSamples->setPlainText(QString::number(value));
}

void DialogStress::editSamples_editingFinished()
{
    bool ok;
    int value = ui->editSamples->toPlainText().toInt(&ok);

    if (ok)
    {
        if (value >= ui->sliderSamples->minimum() && value <= ui->sliderSamples->maximum())
        {
            // slider에도 값 업데이트
            ui->sliderSamples->setValue(value);
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


void DialogStress::sliderIterations_sliderMoved(int position)
{
    int value = ui->sliderIterations->value();
    ui->editIterations->setPlainText(QString::number(value));
}

void DialogStress::editIterations_editingFinished()
{
    bool ok;
    int value = ui->editIterations->toPlainText().toInt(&ok);

    if (ok)
    {
        if (value >= ui->sliderIterations->minimum() && value <= ui->sliderIterations->maximum())
        {
            // slider에도 값 업데이트
            ui->sliderIterations->setValue(value);
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

void DialogStress::EnableUI(bool enable)
{
    ui->cbPresets->setEnabled(enable);
    ui->btnRemovePreset->setEnabled(enable);
    ui->btnSavePreset->setEnabled(enable);
    ui->btnResetPreset->setEnabled(enable);
    ui->sliderRadius->setEnabled(enable);
    ui->editRadius->setEnabled(enable);
    ui->sliderSamples->setEnabled(enable);
    ui->editSamples->setEnabled(enable);
    ui->sliderIterations->setEnabled(enable);
    ui->editIterations->setEnabled(enable);
    ui->chkEnhanceShadows->setEnabled(enable);
}

void DialogStress::UpdatePresetUI(const std::vector<PresetStress>& presets, const int index)
{
    ui->cbPresets->clear();

    if (presets.size() > 0)
    {
        for (const PresetStress& preset : presets)
        {
            QString message = QString("radius: %1, samples: %2, iterations: %3, enhance shadows: %4")
                                  .arg(preset.GetRadius())
                                  .arg(preset.GetSamples())
                                  .arg(preset.GetIterations())
                                  .arg(preset.GetEnhanceShadows() ? "true" : "false");
            ui->cbPresets->addItem(message);
        }
    }

    ui->cbPresets->setCurrentIndex(index);
}
