#ifndef DIALOG_IMAGE_PROCESSING_H
#define DIALOG_IMAGE_PROCESSING_H

#include <QDialog>

namespace Ui {
class DialogImageProcessing;
}

class DialogImageProcessing : public QDialog
{
    Q_OBJECT

public:
    explicit DialogImageProcessing(QWidget *parent = nullptr);
    ~DialogImageProcessing();

private:
    Ui::DialogImageProcessing *ui;
};

#endif // DIALOG_IMAGE_PROCESSING_H
