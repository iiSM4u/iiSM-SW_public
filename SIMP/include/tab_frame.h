#pragma once

#include <QWidget>

namespace Ui {
class TabFrame;
}

class TabFrame : public QWidget
{
    Q_OBJECT

public:
    explicit TabFrame(QWidget *parent = nullptr);
    ~TabFrame();

private:
    Ui::TabFrame *ui;
};
