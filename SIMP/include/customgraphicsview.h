#ifndef CUSTOMGRAPHICSVIEW_H
#define CUSTOMGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>

class CustomGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    explicit CustomGraphicsView(QWidget *parent = nullptr);
    void setImage(const QImage &image);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void mousePositionChanged(int x, int y, const QColor &color);

private:
    QImage image;
    QGraphicsPixmapItem *pixmapItem;
};

#endif // CUSTOMGRAPHICSVIEW_H
