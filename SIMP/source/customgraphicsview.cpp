#include "../include/customgraphicsview.h"

#include <QMouseEvent>

CustomGraphicsView::CustomGraphicsView(QWidget *parent)
    : QGraphicsView(parent), pixmapItem(nullptr)
{
    setMouseTracking(true);  // Enable mouse tracking
}

void CustomGraphicsView::setImage(const QImage &image)
{
    this->image = image;
    QPixmap pixmap = QPixmap::fromImage(image);
    if (pixmapItem)
    {
        scene()->removeItem(pixmapItem);
        delete pixmapItem;
    }
    pixmapItem = scene()->addPixmap(pixmap);
}

void CustomGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    int x = static_cast<int>(scenePos.x());
    int y = static_cast<int>(scenePos.y());

    if (x >= 0 && x < image.width() && y >= 0 && y < image.height())
    {
        QColor color = image.pixelColor(x, y);
        emit mousePositionChanged(x, y, color);
    }
    QGraphicsView::mouseMoveEvent(event);
}
