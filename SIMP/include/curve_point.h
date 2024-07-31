#pragma once

class curve_point
{
public:
    curve_point() = default;
    curve_point(int index, int x, int y)
        : index(index)
        , x(x)
        , y(y)
    {}

    int GetIndex() const { return this->index; }
    int GetX() const { return this->x; }
    int GetY() const { return this->y; }

private:
    int index, x, y;
};

