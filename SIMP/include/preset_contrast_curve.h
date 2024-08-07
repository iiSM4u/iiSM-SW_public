#pragma once

#include "QVector"
#include "QPointF"

class preset_contrast_curve
{
public:
    preset_contrast_curve() = default;
    preset_contrast_curve(const int index, QVector<QPointF>& points)
        : index(index)
        , points(points)
    {}

    int GetIndex() const { return this->index; }
    QVector<QPointF> GetPoints() const { return this->points; }

private:
    int index;
    QVector<QPointF> points;
};

