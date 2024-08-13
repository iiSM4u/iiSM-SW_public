#ifndef DIALOG_IMAGE_PROCESSING_H
#define DIALOG_IMAGE_PROCESSING_H

#include <QDialog>

namespace Ui {
class dialog_image_processing;
}

class dialog_image_processing : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_image_processing(QWidget *parent = nullptr);
    ~dialog_image_processing();

private:
    Ui::dialog_image_processing *ui;
};

#endif // DIALOG_IMAGE_PROCESSING_H
