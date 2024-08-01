#pragma once

class preset_brightness_contrast
{
public:
    preset_brightness_contrast() = default;
    preset_brightness_contrast(const int index, const double brightness, const double contrast)
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

