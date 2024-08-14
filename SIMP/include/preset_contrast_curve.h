#pragma once

#include "QVector"
#include "QPointF"

class PresetContrastCurve
{
public:
    PresetContrastCurve() = default;
    PresetContrastCurve(const int index, QVector<QPointF>& points)
        : index(index)
        , points(points)
    {}

    int GetIndex() const { return this->index; }
    QVector<QPointF> GetPoints() const { return this->points; }

private:
    int index;
    QVector<QPointF> points;
};

