#pragma once

#include <vector>
#include "preset_point.h"

class preset_curve
{
public:
    preset_curve() = default;
    preset_curve(int index, std::vector<preset_point> points)
        : index(index)
        , points(points)
    {}

    int GetIndex() const { return this->index; }
    std::vector<preset_point> GetPoints() const { return this->points; }

private:
    int index;
    std::vector<preset_point> points;
};

