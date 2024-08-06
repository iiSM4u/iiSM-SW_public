#pragma once

#include <QtCharts/QChartView>
#include <QMouseEvent>
#include <QPointF>

class CustomChartView : public QChartView
{
    Q_OBJECT

public:
    explicit CustomChartView(QWidget *parent = nullptr);

signals:
    void chartClicked(const QPointF &point);
    void pointMoved(const QPointF &newPos);
    void pointMovingFinishied();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool pointSelected;
};
