#include "custom_chart_view.h"
#include "constants.h"
#include <QtCharts/QScatterSeries>

CustomChartView::CustomChartView(QWidget *parent)
    : QChartView(parent)
{
}

void CustomChartView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QPointF chartPos = chart()->mapToValue(mapToScene(event->pos()));
        emit chartClicked(chartPos);

        // Check if a point is selected for dragging
        for (const auto &series : chart()->series())
        {
            if (QScatterSeries *scatter = qobject_cast<QScatterSeries *>(series))
            {
                int index = 0;
                for (const QPointF &point : scatter->points())
                {
                    if (qAbs(point.x() - chartPos.x()) < CHART_CLICK_RANGE && qAbs(point.y() - chartPos.y()) < CHART_CLICK_RANGE)
                    {
                        pointSelected = true;
                        selectedPointIndex = index;
                        return;
                    }
                    index++;
                }
            }
        }
    }
    QChartView::mousePressEvent(event);
}

void CustomChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (pointSelected && (event->buttons() & Qt::LeftButton))
    {
        QPointF chartPos = chart()->mapToValue(mapToScene(event->pos()));
        emit pointMoved(selectedPointIndex, chartPos);
    }
    QChartView::mouseMoveEvent(event);
}

void CustomChartView::mouseReleaseEvent(QMouseEvent *event)
{
    pointSelected = false;
    emit pointMovingFinishied();
    QChartView::mouseReleaseEvent(event);
}
