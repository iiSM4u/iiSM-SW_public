#pragma once

#include <QDialog>

namespace Ui {
class dialog_image_curve;
}

class dialog_image_curve : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_image_curve(QWidget *parent = nullptr);
    ~dialog_image_curve();

private:
    Ui::dialog_image_curve *ui;
};
