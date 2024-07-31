#pragma once

#include <vector>
#include "curve_point.h"

class preset_contrast_curve
{
public:
    preset_contrast_curve() = default;
    preset_contrast_curve(int index, std::vector<curve_point> points)
        : index(index)
        , points(points)
    {}

    int GetIndex() const { return this->index; }
    std::vector<curve_point> GetPoints() const { return this->points; }

private:
    int index;
    std::vector<curve_point> points;
};

