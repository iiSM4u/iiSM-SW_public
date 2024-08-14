#include "dialog_stretch_contrast.h"
#include "ui_dialog_stretch_contrast.h"

DialogStretchContrast::DialogStretchContrast(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogStretchContrast)
{
    ui->setupUi(this);
}

DialogStretchContrast::DialogStretchContrast(const bool keepColors, const bool nonLinearComponents, const bool enable, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogStretchContrast)
{
    ui->setupUi(this);

    connect(ui->chkStretchContrast, &QCheckBox::checkStateChanged, this, &DialogStretchContrast::chkStretchContrast_CheckedChanged);

    ui->chkStretchContrast->setCheckState(enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui->chkKeepColors->setCheckState(keepColors ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui->chkNonLinearComponents->setCheckState(nonLinearComponents ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

    DialogStretchContrast::EnableUI(enable);
}

DialogStretchContrast::~DialogStretchContrast()
{
    delete ui;
}

bool DialogStretchContrast::getKeepColors() const
{
    return ui->chkKeepColors->isChecked();
}

bool DialogStretchContrast::getNonLinearComponents() const
{
    return ui->chkNonLinearComponents->isChecked();
}

bool DialogStretchContrast::getEnable() const
{
    return ui->chkStretchContrast->isChecked();
}

void DialogStretchContrast::chkStretchContrast_CheckedChanged(Qt::CheckState checkState)
{
    DialogStretchContrast::EnableUI(checkState == Qt::CheckState::Checked);
}

void DialogStretchContrast::EnableUI(bool enable)
{
    ui->chkKeepColors->setEnabled(enable);
    ui->chkNonLinearComponents->setEnabled(enable);
}
