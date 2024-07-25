#include "dialog_stress.h"
#include "ui_dialog_stress.h"
#include "constants.h"
#include "utils.h"

#include <QMessageBox>
#include <QJsonObject>

dialog_stress::dialog_stress(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialog_stress)
{
    ui->setupUi(this);
}

dialog_stress::dialog_stress(bool enable, int radius, int samples, int iterations, bool enhaceShadows, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialog_stress)
{
    ui->setupUi(this);

    connect(ui->chkStress, &QCheckBox::checkStateChanged, this, &dialog_stress::chkStress_CheckedChanged);
    connect(ui->cbPresets, &QComboBox::currentIndexChanged, this, &dialog_stress::cbPreset_SelectedIndexChanged);
    connect(ui->btnSavePreset, &QPushButton::clicked, this, &dialog_stress::btnSavePreset_Click);

    connect(ui->sliderRadius, &QSlider::sliderMoved, this, &dialog_stress::sliderRadius_sliderMoved);
    connect(ui->editRadius, &CustomPlainTextEdit::editingFinished, this, &dialog_stress::editRadius_editingFinished);
    connect(ui->sliderSamples, &QSlider::sliderMoved, this, &dialog_stress::sliderSamples_sliderMoved);
    connect(ui->editSamples, &CustomPlainTextEdit::editingFinished, this, &dialog_stress::editSamples_editingFinished);
    connect(ui->sliderIterations, &QSlider::sliderMoved, this, &dialog_stress::sliderIterations_sliderMoved);
    connect(ui->editIterations, &CustomPlainTextEdit::editingFinished, this, &dialog_stress::editIterations_editingFinished);

    // set min-max
    ui->sliderRadius->setMinimum(GEGL_STRESS_RADIUS_MIN);
    ui->sliderRadius->setMaximum(GEGL_STRESS_RADIUS_MAX);
    ui->sliderRadius->setValue(radius);
    ui->editRadius->setPlainText(QString::number(radius));

    ui->sliderSamples->setMinimum(GEGL_STRESS_SAMPLES_MIN);
    ui->sliderSamples->setMaximum(GEGL_STRESS_SAMPLES_MAX);
    ui->sliderSamples->setValue(samples);
    ui->editSamples->setPlainText(QString::number(samples));

    ui->sliderIterations->setMinimum(GEGL_STRESS_ITERATIONS_MIN);
    ui->sliderIterations->setMaximum(GEGL_STRESS_ITERATIONS_MAX);
    ui->sliderIterations->setValue(iterations);
    ui->editIterations->setPlainText(QString::number(iterations));

    ui->chkEnhanceShadows->setCheckState(enhaceShadows ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

    ui->chkStress->setCheckState(enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    dialog_stress::EnableUI(enable);

    // load preset
    QString pathPreset = QCoreApplication::applicationDirPath() + PATH_JSON_STRESS;
    QJsonArray jsonArray;

    if (loadJsonFile(pathPreset, jsonArray))
    {
        this->presets = dialog_stress::parseJsonArray(jsonArray);
    }

    dialog_stress::UpdatePresetUI(this->presets);
}

dialog_stress::~dialog_stress()
{
    delete ui;
}


bool dialog_stress::getEnable() const
{
    return ui->chkStress->isChecked();
}

int dialog_stress::getRadius() const
{
    return ui->sliderRadius->value();
}

int dialog_stress::getSamples() const
{
    return ui->sliderSamples->value();
}

int dialog_stress::getIterations() const
{
    return ui->sliderIterations->value();
}

bool dialog_stress::getEnhanceShadows() const
{
    return ui->chkEnhanceShadows->isChecked();
}

void dialog_stress::chkStress_CheckedChanged(Qt::CheckState checkState)
{
    dialog_stress::EnableUI(checkState == Qt::CheckState::Checked);
}

void dialog_stress::cbPreset_SelectedIndexChanged(int index)
{
    if (index > -1)
    {
        preset_stress preset = this->presets[index];

        ui->sliderRadius->setValue(preset.GetRadius());
        ui->editRadius->setPlainText(QString::number(preset.GetRadius()));

        ui->sliderSamples->setValue(preset.GetSamples());
        ui->editSamples->setPlainText(QString::number(preset.GetSamples()));

        ui->sliderIterations->setValue(preset.GetIterations());
        ui->editIterations->setPlainText(QString::number(preset.GetIterations()));

        ui->chkEnhanceShadows->setCheckState(preset.GetEnhanceShadows() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    }
}

void dialog_stress::btnSavePreset_Click()
{
    bool ok;
    int radius = ui->editRadius->toPlainText().toInt(&ok);
    int samples = ui->editSamples->toPlainText().toInt(&ok);
    int iterations = ui->editIterations->toPlainText().toInt(&ok);
    bool enhanceShadows = ui->chkEnhanceShadows->isChecked();
    int index = this->presets.size();

    this->presets.emplace_back(index, radius, samples, iterations, enhanceShadows);

    QJsonArray jsonArray;
    dialog_stress::convertJsonArray(this->presets, jsonArray);

    QString pathPreset = QCoreApplication::applicationDirPath() + PATH_JSON_STRESS;
    saveJsonFile(pathPreset, jsonArray);

    dialog_stress::UpdatePresetUI(this->presets);
}

void dialog_stress::sliderRadius_sliderMoved(int position)
{
    int value = ui->sliderRadius->value();
    ui->editRadius->setPlainText(QString::number(value));
}

void dialog_stress::editRadius_editingFinished()
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
            QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            ui->editRadius->setPlainText(QString::number(ui->sliderRadius->value()));
        }
    }
    else
    {
        QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        ui->editRadius->setPlainText(QString::number(ui->sliderRadius->value()));
    }
}

void dialog_stress::sliderSamples_sliderMoved(int position)
{
    int value = ui->sliderSamples->value();
    ui->editSamples->setPlainText(QString::number(value));
}

void dialog_stress::editSamples_editingFinished()
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
            QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            ui->editSamples->setPlainText(QString::number(ui->sliderSamples->value()));
        }
    }
    else
    {
        QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        ui->editSamples->setPlainText(QString::number(ui->sliderSamples->value()));
    }
}


void dialog_stress::sliderIterations_sliderMoved(int position)
{
    int value = ui->sliderIterations->value();
    ui->editIterations->setPlainText(QString::number(value));
}

void dialog_stress::editIterations_editingFinished()
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
            QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_RANGE);

            // 기존 값으로 되돌린다.
            ui->editIterations->setPlainText(QString::number(ui->sliderIterations->value()));
        }
    }
    else
    {
        QMessageBox::warning(this, TITLE_ERROR, MSG_INVALID_VALUE);

        // 기존 값으로 되돌린다.
        ui->editIterations->setPlainText(QString::number(ui->sliderIterations->value()));
    }
}

void dialog_stress::EnableUI(bool enable)
{
    ui->cbPresets->setEnabled(enable);
    ui->btnSavePreset->setEnabled(enable);
    ui->sliderRadius->setEnabled(enable);
    ui->editRadius->setEnabled(enable);
    ui->sliderSamples->setEnabled(enable);
    ui->editSamples->setEnabled(enable);
    ui->sliderIterations->setEnabled(enable);
    ui->editIterations->setEnabled(enable);
    ui->chkEnhanceShadows->setEnabled(enable);
}

void dialog_stress::UpdatePresetUI(const std::vector<preset_stress>& presets)
{
    ui->cbPresets->clear();

    if (presets.size() > 0)
    {
        for (const preset_stress& preset : presets)
        {
            QString message = QString("radius: %1, samples: %2, iterations: %3, enhance shadows: %4")
                                  .arg(preset.GetRadius())
                                  .arg(preset.GetSamples())
                                  .arg(preset.GetIterations())
                                  .arg(preset.GetEnhanceShadows() ? "true" : "false");
            ui->cbPresets->addItem(message);
        }
    }

    ui->cbPresets->setCurrentIndex(presets.size()-1);
}

std::vector<preset_stress> dialog_stress::parseJsonArray(const QJsonArray& jsonArray)
{
    std::vector<preset_stress> presets;
    for (const QJsonValue& value : jsonArray)
    {
        QJsonObject obj = value.toObject();
        int index = obj[KEY_INDEX].toInt();
        int radius = obj[KEY_RADIUS].toInt();
        int samples = obj[KEY_SAMPLES].toInt();
        int iterations = obj[KEY_ITERATIONS].toInt();
        bool enhanceShadows = obj[KEY_ENHANCE_SHADOWS].toBool();
        presets.emplace_back(index, radius, samples, iterations, enhanceShadows);
    }
    return presets;
}

void dialog_stress::convertJsonArray(const std::vector<preset_stress>& presets, QJsonArray& jsonArray)
{
    for (const preset_stress& preset : presets)
    {
        QJsonObject jsonObject;
        jsonObject[KEY_INDEX] = preset.GetIndex();
        jsonObject[KEY_RADIUS] = preset.GetRadius();
        jsonObject[KEY_SAMPLES] = preset.GetSamples();
        jsonObject[KEY_ITERATIONS] = preset.GetIterations();
        jsonObject[KEY_ENHANCE_SHADOWS] = preset.GetEnhanceShadows();
        jsonArray.append(jsonObject);
    }
}
