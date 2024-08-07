#pragma once

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>

#include "video_format_type.h"
#include "preset_brightness_contrast.h"
#include "preset_stress.h"
#include "preset_contrast_curve.h"

double roundToDecimalPlaces(double value, int decimalPlaces);
bool loadJsonFile(const QString& path, QJsonArray& jsonArray);
bool saveJsonFile(const QString& filePath, const QJsonArray& jsonArray);

QString getVideoExtension(VideoFormatType type);
int getVideoFourcc(VideoFormatType type);

std::vector<preset_brightness_contrast> convertJsonToBrightnessContrastPresets(const QJsonArray& jsonArray);
void convertBrightnessContrastPresetsToJsonArray(const std::vector<preset_brightness_contrast>& presets, QJsonArray& jsonArray);

std::vector<preset_stress> convertJsonToStressPrestes(const QJsonArray& jsonArray);
void convertStressPrestesToJsonArray(const std::vector<preset_stress>& presets, QJsonArray& jsonArray);

std::vector<preset_contrast_curve> convertJsonToPresetsImageCurve(const QJsonArray& jsonArray);
void convertPresetsImageCurveToJsonArray(const std::vector<preset_contrast_curve>& presets, QJsonArray& jsonArray);
