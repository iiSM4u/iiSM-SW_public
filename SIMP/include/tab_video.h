#pragma once

#include <QWidget>
#include <QThread>

namespace Ui {
class TabVideo;
}

class TabVideo : public QWidget
{
    Q_OBJECT

public:
    explicit TabVideo(QWidget *parent = nullptr);
    ~TabVideo();

private:
    Ui::TabVideo *ui;

    QThread threadVideo;
};
