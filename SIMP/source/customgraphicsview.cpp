#include "customgraphicsview.h"
#include <QMouseEvent>

CustomGraphicsView::CustomGraphicsView(QWidget *parent)
    : QGraphicsView(parent), pixmapItem(new QGraphicsPixmapItem()) {
    // Ensure the view has a scene
    QGraphicsScene *scene = new QGraphicsScene(this);
    this->setScene(scene);
    scene->addItem(pixmapItem);
    // Enable mouse tracking
    setMouseTracking(true);
}

void CustomGraphicsView::setImage(const QImage &image) {
    this->image = image;
    pixmapItem->setPixmap(QPixmap::fromImage(image));
    fitInView();
}

void CustomGraphicsView::fitInView() {
    QGraphicsView::fitInView(pixmapItem, Qt::KeepAspectRatio);
}

void CustomGraphicsView::mouseMoveEvent(QMouseEvent *event) {
    QPointF point = mapToScene(event->pos());
    int x = static_cast<int>(point.x());
    int y = static_cast<int>(point.y());
    if (image.rect().contains(x, y)) {
        QColor color = QColor::fromRgb(image.pixel(x, y));
        emit mousePositionChanged(x, y, color);
    }
    qDebug() << "Mouse move event at: (" << x << ", " << y << ")";
    QGraphicsView::mouseMoveEvent(event);
}
