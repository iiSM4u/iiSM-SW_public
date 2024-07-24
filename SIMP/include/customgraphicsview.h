#pragma once

#include <QGraphicsView>
#include <QGraphicsPixmapItem>

class CustomGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    explicit CustomGraphicsView(QWidget *parent = nullptr);
    void setImage(const QImage &image);
    void fitInView();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void mousePositionChanged(int x, int y, const QColor &color);

private:
    QImage image;
    QGraphicsPixmapItem *pixmapItem;
};
