#include "dialog_curve_point.h"
#include "ui_dialog_curve_point.h"
#include "simp_const_menu.h"
#include "QMessageBox"

DialogCurvePoint::DialogCurvePoint(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogCurvePoint)
{
    ui->setupUi(this);
}

DialogCurvePoint::DialogCurvePoint(const QVector<QPointF>& qpoints, const int x, const int y, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogCurvePoint)
    , qpoints(qpoints)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DialogCurvePoint::onOkClicked);

    ui->spinInput->setValue(x);
    ui->spinOutput->setValue(y);
}

DialogCurvePoint::~DialogCurvePoint()
{
    delete ui;
}

int DialogCurvePoint::getInputValue() const
{
    return ui->spinInput->value();
}

int DialogCurvePoint::getOutputValue() const
{
    return ui->spinOutput->value();
}

void DialogCurvePoint::onOkClicked()
{
    int x = ui->spinInput->value();
    int y = ui->spinOutput->value();

    for (const QPointF& point : this->qpoints)
    {
        if (std::abs(point.x() - x) <= std::numeric_limits<double>::epsilon() &&
            std::abs(point.y() - y) <= std::numeric_limits<double>::epsilon())
        {
            QMessageBox::warning(this, SimpConstMenu::TITLE_ERROR, SimpConstMenu::MSG_EXISTS_VALUE);
            return;
        }
    }
    accept();
}
