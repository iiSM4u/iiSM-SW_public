#ifndef DIALOG_RECORD_OPTION_H
#define DIALOG_RECORD_OPTION_H

#include <QDialog>

namespace Ui {
class dialog_record_option;
}

class dialog_record_option : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_record_option(QWidget *parent = nullptr);
    ~dialog_record_option();

private:
    Ui::dialog_record_option *ui;
};

#endif // DIALOG_RECORD_OPTION_H
