#ifndef DIALOG_CURVE_POINT_H
#define DIALOG_CURVE_POINT_H

#include <QDialog>

namespace Ui {
class dialog_curve_point;
}

class dialog_curve_point : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_curve_point(QWidget *parent = nullptr);
    ~dialog_curve_point();

private:
    Ui::dialog_curve_point *ui;
};

#endif // DIALOG_CURVE_POINT_H
