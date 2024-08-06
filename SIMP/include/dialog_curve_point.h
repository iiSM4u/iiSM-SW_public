#pragma once

#include <QDialog>

namespace Ui {
class dialog_curve_point;
}

class dialog_curve_point : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_curve_point(QWidget *parent = nullptr);
    dialog_curve_point(const QVector<QPointF>& points, const int x, const int y, QWidget *parent = nullptr);
    ~dialog_curve_point();

    int getInputValue() const;
    int getOutputValue() const;

private slots:
    void onOkClicked();
    //void onCancelClicked();

private:
    Ui::dialog_curve_point *ui;
    QVector<QPointF> qpoints;
};
