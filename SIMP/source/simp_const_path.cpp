#include "simp_const_path.h"
#include <QStandardPaths>

const QString SimpConstPath::DIR_SIMP = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/SIMP";
const QString SimpConstPath::DIR_CAPTURE_FRAME = SimpConstPath::DIR_SIMP + "/captures";
const QString SimpConstPath::DIR_RECORD_VIDEO = SimpConstPath::DIR_SIMP + "/videos";
const QString SimpConstPath::DIR_PRESET = SimpConstPath::DIR_SIMP + "/presets";
const QString SimpConstPath::DIR_LOG = SimpConstPath::DIR_SIMP + "/logs";

const QString SimpConstPath::PATH_JSON_CONTRAST_CURVE = SimpConstPath::DIR_PRESET + "/contrast_curve.json";
const QString SimpConstPath::PATH_JSON_IMAGE_PROCESSING = SimpConstPath::DIR_PRESET + "/image_processing.json";
//const QString SimpConstPath::PATH_JSON_BRIGHTNESS_CONTRAST = DIR_PRESET + "/brightness_contrast.json";
//const QString SimpConstPath::PATH_JSON_STRESS = DIR_PRESET + "/stress.json";

const QString SimpConstPath::PATH_LOG = SimpConstPath::DIR_LOG + "/log.txt";

const QString SimpConstPath::EXTENSION_CAPTURE_IMAGE = ".png";
