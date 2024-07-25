#pragma once

#include <qstring.h>

const float ZOOM_VALUE = 0.25f;
const float ZOOM_MAX = 3.0f;
const float ZOOM_MIN = 0.25f;

const int FRAME_PER_SECOND = 30;

const double GEGL_CONTRAST_DEFAULT = 1.00;
const double GEGL_CONTRAST_MIN = -5.00;
const double GEGL_CONTRAST_MAX = 5.00;

const double GEGL_BRIGHTNESS_DEFAULT = 0.00;
const double GEGL_BRIGHTNESS_MIN = -3.00;
const double GEGL_BRIGHTNESS_MAX = 3.00;

const bool GEGL_STRETCH_CONTRAST_KEEP_COLORS_DEFAULT = true;
const bool GEGL_STRETCH_CONTRAST_PERCEPTUAL_DEFAULT = false;

const int GEGL_STRESS_RADIUS_DEFAULT = 300;
const int GEGL_STRESS_RADIUS_MIN = 2;
const int GEGL_STRESS_RADIUS_MAX = 6000;

const int GEGL_STRESS_SAMPLES_DEFAULT = 5;
const int GEGL_STRESS_SAMPLES_MIN = 2;
const int GEGL_STRESS_SAMPLES_MAX = 500;

const int GEGL_STRESS_INTERATIONS_DEFAULT = 5;
const int GEGL_STRESS_INTERATIONS_MIN = 1;
const int GEGL_STRESS_INTERATIONS_MAX = 1000;

const bool GEGL_STRESS_ENHANCE_SHADOWS_DEFAULT = false;

const int GEGL_CONTRAST_CURVE_SAMPLING_POINTS_DEFAULT = 0;
const int GEGL_CONTRAST_CURVE_SAMPLING_POINTS_MIN = 0;
const int GEGL_CONTRAST_CURVE_SAMPLING_POINTS_MAX = 65536;

const QString DIR_CAPTURE_FRAME = "/captures";
const QString DIR_RECORD_VIDEO = "/videos";
const QString DIR_PRESET = "/presets";

const QString PATH_JSON_BRIGHTNESS_CONTRAST = DIR_PRESET + "/brightness_contrast.json";
const QString PATH_JSON_STRESS = DIR_PRESET + "/stress.json";
const QString PATH_JSON_CONTRAST_CURVE = DIR_PRESET + "/contrast_curve.json";

const QString EXTENSION_CAPTURE_IMAGE = ".png";
const QString FORMAT_DATE_TIME = "yyyy_MM_dd_hh_mm_ss";


const QString KEY_INDEX = "index";
const QString KEY_BRIGHTNESS = "brightness";
const QString KEY_CONTRAST = "contrast";


const QString BTN_PLAY = "Play";
const QString MENU_PAUSE = "Pause";

const QString TITLE_ERROR = "Error";
const QString MSG_INVALID_VALUE = "Invalid Value";
const QString MSG_INVALID_RANGE = "Invalid Range";
