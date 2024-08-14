#pragma once

#include <QDialog>

namespace Ui {
class DialogStretchContrast;
}

class DialogStretchContrast : public QDialog
{
    Q_OBJECT

public:
    explicit DialogStretchContrast(QWidget *parent = nullptr);
    DialogStretchContrast(const bool keepColors, const bool nonLinearComponents, const bool enable, QWidget *parent = nullptr);
    ~DialogStretchContrast();

    bool getKeepColors() const;
    bool getNonLinearComponents() const;
    bool getEnable() const;

private slots:
    void chkStretchContrast_CheckedChanged(Qt::CheckState checkState);

    void EnableUI(bool enable);

private:
    Ui::DialogStretchContrast *ui;
};
