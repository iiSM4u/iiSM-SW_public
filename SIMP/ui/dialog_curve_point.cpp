#include "dialog_curve_point.h"
#include "ui_dialog_curve_point.h"
#include "constants.h"
#include "QMessageBox"

dialog_curve_point::dialog_curve_point(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialog_curve_point)
{
    ui->setupUi(this);
}

dialog_curve_point::dialog_curve_point(const QVector<QPointF>& qpoints, const int x, const int y, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialog_curve_point)
    , qpoints(qpoints)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &dialog_curve_point::onOkClicked);
    //connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &dialog_curve_point::onCancelClicked);

    ui->spinInput->setValue(x);
    ui->spinOutput->setValue(y);
}

dialog_curve_point::~dialog_curve_point()
{
    delete ui;
}

int dialog_curve_point::getInputValue() const
{
    return ui->spinInput->value();
}

int dialog_curve_point::getOutputValue() const
{
    return ui->spinOutput->value();
}

void dialog_curve_point::onOkClicked()
{
    int x = ui->spinInput->value();
    int y = ui->spinOutput->value();

    for (const QPointF& point : this->qpoints)
    {
        if (std::abs(point.x() - x) <= std::numeric_limits<double>::epsilon() &&
            std::abs(point.y() - y) <= std::numeric_limits<double>::epsilon())
        {
            QMessageBox::warning(this, TITLE_ERROR, MSG_EXISTS_VALUE);
            return;
        }
    }
    accept();
}
