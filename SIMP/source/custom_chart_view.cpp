#include "custom_chart_view.h"
#include "simp_const_value.h"
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

        int min = SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MIN + SimpConstValue::MARGIN_CHART_CLICK;
        int max = SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MAX - SimpConstValue::MARGIN_CHART_CLICK;

        // Check if a point is selected for dragging
        for (const auto &series : chart()->series())
        {
            if (QScatterSeries *scatter = qobject_cast<QScatterSeries *>(series))
            {
                int index = 0;
                for (const QPointF &point : scatter->points())
                {
                    // 클릭 시에는 (0, 0), (255, 255)의 점은 선택 못하도록
                    if ((point.x() > min || point.y() > min) &&
                        (point.x() < max || point.y() < max) &&
                        qAbs(point.x() - chartPos.x()) < SimpConstValue::MARGIN_CHART_CLICK && qAbs(point.y() - chartPos.y()) < SimpConstValue::MARGIN_CHART_CLICK)
                    {
                        pointSelected = true;
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
        emit pointMoved(chartPos);
    }
    QChartView::mouseMoveEvent(event);
}

void CustomChartView::mouseReleaseEvent(QMouseEvent *event)
{
    pointSelected = false;
    emit pointMovingFinishied();
    QChartView::mouseReleaseEvent(event);
}
