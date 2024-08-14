#pragma once

#include <QDialog>

namespace Ui {
class DialogCurvePoint;
}

class DialogCurvePoint : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCurvePoint(QWidget *parent = nullptr);
    DialogCurvePoint(const QVector<QPointF>& points, const int x, const int y, QWidget *parent = nullptr);
    ~DialogCurvePoint();

    int getInputValue() const;
    int getOutputValue() const;

private slots:
    void onOkClicked();
    //void onCancelClicked();

private:
    Ui::DialogCurvePoint *ui;
    QVector<QPointF> qpoints;
};
