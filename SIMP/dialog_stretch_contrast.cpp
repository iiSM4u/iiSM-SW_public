#include "dialog_stretch_contrast.h"
#include "ui_dialog_stretch_contrast.h"

dialog_stretch_contrast::dialog_stretch_contrast(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialog_stretch_contrast)
{
    ui->setupUi(this);
}

dialog_stretch_contrast::dialog_stretch_contrast(const bool keepColors, const bool nonLinearComponents, const bool enable, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialog_stretch_contrast)
{
    ui->setupUi(this);

    connect(ui->chkStretchContrast, &QCheckBox::checkStateChanged, this, &dialog_stretch_contrast::chkStretchContrast_CheckedChanged);

    ui->chkStretchContrast->setCheckState(enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui->chkKeepColors->setCheckState(keepColors ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui->chkNonLinearComponents->setCheckState(nonLinearComponents ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

    dialog_stretch_contrast::EnableUI(enable);
}

dialog_stretch_contrast::~dialog_stretch_contrast()
{
    delete ui;
}

bool dialog_stretch_contrast::getKeepColors() const
{
    return ui->chkKeepColors->isChecked();
}

bool dialog_stretch_contrast::getNonLinearComponents() const
{
    return ui->chkNonLinearComponents->isChecked();
}

bool dialog_stretch_contrast::getEnable() const
{
    return ui->chkStretchContrast->isChecked();
}

void dialog_stretch_contrast::chkStretchContrast_CheckedChanged(Qt::CheckState checkState)
{
    dialog_stretch_contrast::EnableUI(checkState == Qt::CheckState::Checked);
}

void dialog_stretch_contrast::EnableUI(bool enable)
{
    ui->chkKeepColors->setEnabled(enable);
    ui->chkNonLinearComponents->setEnabled(enable);
}
