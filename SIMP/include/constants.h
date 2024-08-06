#pragma once

#include <qstring.h>

const float ZOOM_VALUE = 0.25f;
const float ZOOM_MAX = 3.0f;
const float ZOOM_MIN = 0.25f;

const int DELAY_CAMERA = 30;
const int DELAY_VIDEO = 30;

const int RECORD_FRAME_RATE_DEFAULT = 30;
const int RECORD_FRAME_RATE_MIN = 1;
const int RECORD_FRAME_RATE_MAX = 60;

const int RECORD_QUALITY_DEFAULT = 75;

const int MARGIN_CHART_CLICK = 20;

const int MIICAM_EXPOGAIN_MAX = 5000;
const double MIICAM_EXPOSURE_TIME_MIN = 0.1;
const double MIICAM_EXPOSURE_TIME_MAX = 5000.0;
const double MIICAM_TEMPERATURE_MIN = -50.0;
const double MIICAM_TEMPERATURE_MAX = 40.0;
const int MIICAM_DARK_FIELD_QUANTITY_MIN = 1;
const int MIICAM_DARK_FIELD_QUANTITY_MAX = 100;
const int MIICAM_DARK_FIELD_QUANTITY_DEFAULT = 5;

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

const int GEGL_STRESS_ITERATIONS_DEFAULT = 5;
const int GEGL_STRESS_ITERATIONS_MIN = 1;
const int GEGL_STRESS_ITERATIONS_MAX = 1000;

const bool GEGL_STRESS_ENHANCE_SHADOWS_DEFAULT = false;

const int GEGL_CONTRAST_CURVE_SAMPLING_POINTS_DEFAULT = 0;
const int GEGL_CONTRAST_CURVE_SAMPLING_POINTS_MIN = 0;
const int GEGL_CONTRAST_CURVE_SAMPLING_POINTS_MAX = 65536;

const int GEGL_CONTRAST_CURVE_VALUE_MIN = 0;
const int GEGL_CONTRAST_CURVE_VALUE_MAX = 255;

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
const QString KEY_RADIUS = "radius";
const QString KEY_SAMPLES = "samples";
const QString KEY_ITERATIONS = "iterations";
const QString KEY_ENHANCE_SHADOWS = "enhance_shadows";
const QString KEY_POINTS = "points";
const QString KEY_POS_X = "pos_x";
const QString KEY_POS_Y = "pos_y";

const QString KEY_NONE = "none";

const QString BTN_PLAY = "Play";
const QString BTN_PAUSE = "Pause";
const QString BTN_RECORD_ON = "Record On";
const QString BTN_RECORD_OFF = "Record Off";

const QString TITLE_ERROR = "Error";

const QString MSG_INVALID_VALUE = "Invalid Value";
const QString MSG_INVALID_RANGE = "Invalid Range";
const QString MSG_FILE_OPEN_ERROR = "File open error";
const QString MSG_EXISTS_VALUE = "Exists Value";
