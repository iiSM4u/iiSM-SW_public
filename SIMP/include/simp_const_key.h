#pragma once

#include <qstring.h>

class SimpConstKey
{
public:
    static const QString INDEX;
    static const QString NONE;
    static const QString POS_X;
    static const QString POS_Y;
    static const QString POINTS;

    static const QString BRIGHTNESS_CONTRAST_ENABLE;
    static const QString BRIGHTNESS;
    static const QString CONTRAST;

    static const QString STRESS_ENABLE;
    static const QString STRESS_RADIUS;
    static const QString STRESS_SAMPLES;
    static const QString STRESS_ITERATIONS;
    static const QString STRESS_ENHANCE_SHADOWS;

    static const QString STRETCH_CONTRAST_ENABLE;
    static const QString STRETCH_CONTRAST_KEEP_COLORS;
    static const QString STRETCH_CONTRAST_NON_LINEAR_COMPONENTS;
};
