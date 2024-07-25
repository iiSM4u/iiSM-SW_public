#ifndef FRM_BRIGHTNESS_CONTRAST_H
#define FRM_BRIGHTNESS_CONTRAST_H

#include <QDialog>

namespace Ui {
class frm_brightness_contrast;
}

class frm_brightness_contrast : public QDialog
{
    Q_OBJECT

public:
    explicit frm_brightness_contrast(QWidget *parent = nullptr);
    ~frm_brightness_contrast();

private:
    Ui::frm_brightness_contrast *ui;
};

#endif // FRM_BRIGHTNESS_CONTRAST_H
