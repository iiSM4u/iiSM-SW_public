#pragma once

class PresetImageProcessing
{
public:
    PresetImageProcessing() = default;
    PresetImageProcessing(
        const int index
        , const bool brightnessContrastEnable
        , const double brightness
        , const double contrast
        , const bool stressEnable
        , const int stressRadius
        , const int stressSamples
        , const int stressIterations
        , const bool stressEnhanceShadows
        , const bool stretchContrastEnable
        , const bool stretchContrastKeepColors
        , const bool stretchContrastNonLinearComponents
    )
        : index(index)
        , brightnessContrastEnable(brightnessContrastEnable)
        , brightenss(brightness)
        , contrast(contrast)
        , stressEnable(stressEnable)
        , stressRadius(stressRadius)
        , stressSamples(stressSamples)
        , stressIterations(stressIterations)
        , stressEnhanceShadows(stressEnhanceShadows)
        , stretchContrastEnable(stretchContrastEnable)
        , stretchContrastKeepColors(stretchContrastKeepColors)
        , stretchContrastNonLinearComponents(stretchContrastNonLinearComponents)
    {}

    int GetIndex() const { return this->index; }

    bool GetBrightnessContrastEnable() const { return this->brightnessContrastEnable; }
    double GetBrightness() const { return this->brightenss; }
    double GetContrast() const { return this->contrast; }

    bool GetStressEnable() const { return this->stressEnable; }
    int GetStressRadius() const { return this->stressRadius; }
    int GetStressSamples() const { return this->stressSamples; }
    int GetStressIterations() const { return this->stressIterations; }
    bool GetStressEnhanceShadows() const { return this->stressEnhanceShadows; }

    bool GetStretchContrastEnable() const { return this->stretchContrastEnable; }
    bool GetStretchContrastKeepColors() const { return this->stretchContrastKeepColors; }
    bool GetStretchContrastNonLinearComponents() const { return this->stretchContrastNonLinearComponents; }

private:
    bool brightnessContrastEnable, stressEnable, stretchContrastEnable,
        stressEnhanceShadows, stretchContrastKeepColors, stretchContrastNonLinearComponents;
    int index, stressRadius, stressSamples, stressIterations;
    double brightenss, contrast;
};

