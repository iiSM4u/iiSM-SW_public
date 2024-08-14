#pragma once

class PresetBrightnessContrast
{
public:
    PresetBrightnessContrast() = default;
    PresetBrightnessContrast(const int index, const double brightness, const double contrast)
        : index(index)
        , brightenss(brightness)
        , contrast(contrast)
    {}

    int GetIndex() const { return this->index; }
    double GetBrightness() const { return this->brightenss; }
    double GetContrast() const { return this->contrast; }

private:
    int index;
    double brightenss, contrast;
};

