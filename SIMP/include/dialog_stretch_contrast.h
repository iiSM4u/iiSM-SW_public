#pragma once

#include <QDialog>

namespace Ui {
class dialog_stretch_contrast;
}

class dialog_stretch_contrast : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_stretch_contrast(QWidget *parent = nullptr);
    dialog_stretch_contrast(const bool keepColors, const bool nonLinearComponents, const bool enable, QWidget *parent = nullptr);
    ~dialog_stretch_contrast();

    bool getKeepColors() const;
    bool getNonLinearComponents() const;
    bool getEnable() const;

private slots:
    void chkStretchContrast_CheckedChanged(Qt::CheckState checkState);

    void EnableUI(bool enable);

private:
    Ui::dialog_stretch_contrast *ui;
};
