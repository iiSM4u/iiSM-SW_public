#pragma once

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QImage>

#include <opencv2/opencv.hpp>

#include "video_format_type.h"
#include "preset_brightness_contrast.h"
#include "preset_stress.h"
#include "preset_contrast_curve.h"
#include "preset_image_processing.h"


class SimpUtil
{
public:
    static double roundToDecimalPlaces(double value, int decimalPlaces);
    static bool loadJsonFile(const QString& path, QJsonArray& jsonArray);
    static bool saveJsonFile(const QString& filePath, const QJsonArray& jsonArray);

    static QString getVideoExtension(VideoFormatType type);
    static VideoFormatType getVideoFormat(QString extension);

    static int getVideoFourcc(VideoFormatType type);
    static void WriteVideo(const std::vector<cv::Mat>& frames, const VideoFormatType format, const double frameRate, const int quality, const QString filePath);
    static void WriteVideo(const std::vector<QImage>& frames, const VideoFormatType format, const double frameRate, const int quality, const QString filePath);

    static std::vector<PresetBrightnessContrast> convertJsonToBrightnessContrastPresets(const QJsonArray& jsonArray);
    static void convertBrightnessContrastPresetsToJsonArray(const std::vector<PresetBrightnessContrast>& presets, QJsonArray& jsonArray);

    static std::vector<PresetStress> convertJsonToStressPrestes(const QJsonArray& jsonArray);
    static void convertStressPrestesToJsonArray(const std::vector<PresetStress>& presets, QJsonArray& jsonArray);

    static std::vector<PresetContrastCurve> convertJsonToPresetsImageCurve(const QJsonArray& jsonArray);
    static void convertPresetsImageCurveToJsonArray(const std::vector<PresetContrastCurve>& presets, QJsonArray& jsonArray);

    static std::vector<PresetImageProcessing> convertJsonToPresetsImageProcessing(const QJsonArray& jsonArray);
    static void convertPresetsImageProcessingToJsonArray(const std::vector<PresetImageProcessing>& presets, QJsonArray& jsonArray);
};
