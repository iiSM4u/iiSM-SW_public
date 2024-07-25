#pragma once

class preset_point
{
public:
    preset_point() = default;
    preset_point(int index, int x, int y)
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

