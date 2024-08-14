#pragma once

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>

#include "video_format_type.h"
#include "preset_brightness_contrast.h"
#include "preset_stress.h"
#include "preset_contrast_curve.h"


class SimpUtil
{
public:
    static double roundToDecimalPlaces(double value, int decimalPlaces);
    static bool loadJsonFile(const QString& path, QJsonArray& jsonArray);
    static bool saveJsonFile(const QString& filePath, const QJsonArray& jsonArray);

    static QString getVideoExtension(VideoFormatType type);
    static int getVideoFourcc(VideoFormatType type);

    static std::vector<preset_brightness_contrast> convertJsonToBrightnessContrastPresets(const QJsonArray& jsonArray);
    static void convertBrightnessContrastPresetsToJsonArray(const std::vector<preset_brightness_contrast>& presets, QJsonArray& jsonArray);

    static std::vector<preset_stress> convertJsonToStressPrestes(const QJsonArray& jsonArray);
    static void convertStressPrestesToJsonArray(const std::vector<preset_stress>& presets, QJsonArray& jsonArray);

    static std::vector<preset_contrast_curve> convertJsonToPresetsImageCurve(const QJsonArray& jsonArray);
    static void convertPresetsImageCurveToJsonArray(const std::vector<preset_contrast_curve>& presets, QJsonArray& jsonArray);
};
