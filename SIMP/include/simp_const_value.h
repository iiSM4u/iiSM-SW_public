#pragma once

class SimpConstValue
{
public:
    static constexpr float ZOOM_VALUE = 0.25f;
    static constexpr float ZOOM_MAX = 3.0f;
    static constexpr float ZOOM_MIN = 0.25f;

    static constexpr int DELAY_CAMERA = 30;
    static constexpr int DELAY_VIDEO = 30;

    static constexpr int RECORD_FRAME_RATE_DEFAULT = 30;
    static constexpr int RECORD_FRAME_RATE_MIN = 1;
    static constexpr int RECORD_FRAME_RATE_MAX = 60;
    static constexpr int RECORD_QUALITY_DEFAULT = 75;

    static constexpr int MARGIN_CHART_CLICK = 20;

    static constexpr int MIICAM_EXPOGAIN_MAX = 5000;

    static constexpr double MIICAM_EXPOSURE_TIME_MIN = 0.1;
    static constexpr double MIICAM_EXPOSURE_TIME_MAX = 5000.0;

    static constexpr double MIICAM_TEMPERATURE_MIN = -50.0;
    static constexpr double MIICAM_TEMPERATURE_MAX = 40.0;

    static constexpr int MIICAM_DARK_FIELD_QUANTITY_MIN = 1;
    static constexpr int MIICAM_DARK_FIELD_QUANTITY_MAX = 100;
    static constexpr int MIICAM_DARK_FIELD_QUANTITY_DEFAULT = 5;

    static constexpr double GEGL_CONTRAST_DEFAULT = 1.00;
    static constexpr double GEGL_CONTRAST_MIN = -5.00;
    static constexpr double GEGL_CONTRAST_MAX = 5.00;

    static constexpr double GEGL_BRIGHTNESS_DEFAULT = 0.00;
    static constexpr double GEGL_BRIGHTNESS_MIN = -3.00;
    static constexpr double GEGL_BRIGHTNESS_MAX = 3.00;

    static constexpr bool GEGL_STRETCH_CONTRAST_KEEP_COLORS_DEFAULT = true;
    static constexpr bool GEGL_STRETCH_CONTRAST_PERCEPTUAL_DEFAULT = false;

    static constexpr int GEGL_STRESS_RADIUS_DEFAULT = 300;
    static constexpr int GEGL_STRESS_RADIUS_MIN = 2;
    static constexpr int GEGL_STRESS_RADIUS_MAX = 6000;

    static constexpr int GEGL_STRESS_SAMPLES_DEFAULT = 5;
    static constexpr int GEGL_STRESS_SAMPLES_MIN = 2;
    static constexpr int GEGL_STRESS_SAMPLES_MAX = 500;

    static constexpr int GEGL_STRESS_ITERATIONS_DEFAULT = 5;
    static constexpr int GEGL_STRESS_ITERATIONS_MIN = 1;
    static constexpr int GEGL_STRESS_ITERATIONS_MAX = 1000;

    static constexpr bool GEGL_STRESS_ENHANCE_SHADOWS_DEFAULT = false;

    static constexpr int GEGL_CONTRAST_CURVE_SAMPLING_POINTS_DEFAULT = 0;
    static constexpr int GEGL_CONTRAST_CURVE_SAMPLING_POINTS_MIN = 0;
    static constexpr int GEGL_CONTRAST_CURVE_SAMPLING_POINTS_MAX = 65536;

    static constexpr double GEGL_CONTRAST_CURVE_VALUE_MIN = 0.0;
    static constexpr double GEGL_CONTRAST_CURVE_VALUE_MAX = 255.0;
};
