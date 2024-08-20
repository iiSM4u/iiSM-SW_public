#include "simp_util.h"
#include "simp_const_key.h"

#include <QPointF>
#include <QJsonDocument>
#include <QDir>
#include <opencv2/opencv.hpp>

double SimpUtil::roundToDecimalPlaces(double value, int decimalPlaces) {
    double factor = std::pow(10.0, decimalPlaces);
    return std::round(value * factor) / factor;
}

bool SimpUtil::loadJsonFile(const QString& filePath, QJsonArray& jsonArray)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file for reading:" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    QJsonDocument document = QJsonDocument::fromJson(data);
    if (!document.isArray()) {
        qWarning() << "JSON document is not an array";
        return false;
    }

    jsonArray = document.array();
    return true;
}

bool SimpUtil::saveJsonFile(const QString& filePath, const QJsonArray& jsonArray)
{
    // 파일 경로에서 디렉토리를 추출
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.dir();

    // 디렉토리가 존재하지 않으면 생성
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "Could not create directory:" << dir.absolutePath();
            return false;
        }
    }

    QJsonDocument jsonDocument(jsonArray);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open file for writing:" << filePath;
        return false;
    }

    file.write(jsonDocument.toJson(QJsonDocument::Indented));
    file.close();

    return true;
}

QString SimpUtil::getVideoExtension(VideoFormatType type)
{
    switch (type)
    {
    case VideoFormatType::MJPEG: return ".avi";
    case VideoFormatType::XVID: return ".avi";
    case VideoFormatType::MP4V: return ".mp4";
    //case VideoFormatType::NONE : return ".wmv";
    default: return ".wmv";
    }
}


int SimpUtil::getVideoFourcc(VideoFormatType type)
{
    switch (type)
    {
    case VideoFormatType::MJPEG: return cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
    case VideoFormatType::XVID: return cv::VideoWriter::fourcc('X', 'V', 'I', 'G');
    case VideoFormatType::MP4V: return cv::VideoWriter::fourcc('M', 'P', '4', 'V');
    //case VideoFormatType::NONE : return 0;
    default: return 0;
    }
}

std::vector<PresetBrightnessContrast> SimpUtil::convertJsonToBrightnessContrastPresets(const QJsonArray& jsonArray)
{
    std::vector<PresetBrightnessContrast> presets;
    for (const QJsonValue& value : jsonArray)
    {
        QJsonObject obj = value.toObject();
        int index = obj[SimpConstKey::INDEX].toInt();
        double brightness = obj[SimpConstKey::BRIGHTNESS].toDouble();
        double contrast = obj[SimpConstKey::CONTRAST].toDouble();
        presets.emplace_back(index, brightness, contrast);
    }
    return presets;
}

void SimpUtil::convertBrightnessContrastPresetsToJsonArray(const std::vector<PresetBrightnessContrast>& presets, QJsonArray& jsonArray)
{
    for (const PresetBrightnessContrast& preset : presets)
    {
        QJsonObject jsonObject;
        jsonObject[SimpConstKey::INDEX] = preset.GetIndex();
        jsonObject[SimpConstKey::BRIGHTNESS] = preset.GetBrightness();
        jsonObject[SimpConstKey::CONTRAST] = preset.GetContrast();
        jsonArray.append(jsonObject);
    }
}

std::vector<PresetStress> SimpUtil::convertJsonToStressPrestes(const QJsonArray& jsonArray)
{
    std::vector<PresetStress> presets;
    for (const QJsonValue& value : jsonArray)
    {
        QJsonObject obj = value.toObject();
        int index = obj[SimpConstKey::INDEX].toInt();
        int radius = obj[SimpConstKey::STRESS_RADIUS].toInt();
        int samples = obj[SimpConstKey::STRESS_SAMPLES].toInt();
        int iterations = obj[SimpConstKey::STRESS_ITERATIONS].toInt();
        bool enhanceShadows = obj[SimpConstKey::STRESS_ENHANCE_SHADOWS].toBool();
        presets.emplace_back(index, radius, samples, iterations, enhanceShadows);
    }
    return presets;
}

void SimpUtil::convertStressPrestesToJsonArray(const std::vector<PresetStress>& presets, QJsonArray& jsonArray)
{
    for (const PresetStress& preset : presets)
    {
        QJsonObject jsonObject;
        jsonObject[SimpConstKey::INDEX] = preset.GetIndex();
        jsonObject[SimpConstKey::STRESS_RADIUS] = preset.GetRadius();
        jsonObject[SimpConstKey::STRESS_SAMPLES] = preset.GetSamples();
        jsonObject[SimpConstKey::STRESS_ITERATIONS] = preset.GetIterations();
        jsonObject[SimpConstKey::STRESS_ENHANCE_SHADOWS] = preset.GetEnhanceShadows();
        jsonArray.append(jsonObject);
    }
}

std::vector<PresetContrastCurve> SimpUtil::convertJsonToPresetsImageCurve(const QJsonArray& jsonArray)
{
    std::vector<PresetContrastCurve> presets;
    for (const QJsonValue& value : jsonArray)
    {
        QJsonObject obj = value.toObject();
        int index = obj[SimpConstKey::INDEX].toInt();

        QVector<QPointF> points;
        for (const QJsonValue& pointValue : obj[SimpConstKey::POINTS].toArray())
        {
            QJsonObject point = pointValue.toObject();
            double x = point[SimpConstKey::POS_X].toDouble();
            double y = point[SimpConstKey::POS_Y].toDouble();

            points.emplace_back(x, y);
        }

        presets.emplace_back(index, points);
    }
    return presets;
}

void SimpUtil::convertPresetsImageCurveToJsonArray(const std::vector<PresetContrastCurve>& presets, QJsonArray& jsonArray)
{
    for (const PresetContrastCurve& preset : presets)
    {
        QJsonArray pointsArray;

        for (const QPointF& point : preset.GetPoints())
        {
            QJsonObject pointObj;
            pointObj[SimpConstKey::POS_X] = point.x();
            pointObj[SimpConstKey::POS_Y] = point.y();

            pointsArray.append(pointObj);
        }

        QJsonObject jsonObject;
        jsonObject[SimpConstKey::INDEX] = preset.GetIndex();
        jsonObject[SimpConstKey::POINTS] = pointsArray;

        jsonArray.append(jsonObject);
    }
}

std::vector<PresetImageProcessing> SimpUtil::convertJsonToPresetsImageProcessing(const QJsonArray& jsonArray)
{
    std::vector<PresetImageProcessing> presets;
    for (const QJsonValue& value : jsonArray)
    {
        QJsonObject obj = value.toObject();
        int index = obj[SimpConstKey::INDEX].toInt();

        bool brightnessContrastEnable = obj[SimpConstKey::BRIGHTNESS_CONTRAST_ENABLE].toBool();
        double brightness = obj[SimpConstKey::BRIGHTNESS].toDouble();
        double contrast = obj[SimpConstKey::CONTRAST].toDouble();

        bool stressEnable = obj[SimpConstKey::STRESS_ENABLE].toBool();
        int stressRadius = obj[SimpConstKey::STRESS_RADIUS].toInt();
        int stressSamples = obj[SimpConstKey::STRESS_SAMPLES].toInt();
        int stressIterations = obj[SimpConstKey::STRESS_ITERATIONS].toInt();
        bool stressEnhanceShadows = obj[SimpConstKey::STRESS_ENHANCE_SHADOWS].toBool();

        bool stretchContrastEnable = obj[SimpConstKey::STRETCH_CONTRAST_ENABLE].toBool();
        bool stretchContrastKeepColors = obj[SimpConstKey::STRETCH_CONTRAST_KEEP_COLORS].toBool();
        bool stretchContrastNonLinearComponents = obj[SimpConstKey::STRETCH_CONTRAST_NON_LINEAR_COMPONENTS].toBool();

        presets.emplace_back(
            index
            , brightnessContrastEnable
            , brightness
            , contrast
            , stressEnable
            , stressRadius
            , stressSamples
            , stressIterations
            , stressEnhanceShadows
            , stretchContrastEnable
            , stretchContrastKeepColors
            , stretchContrastNonLinearComponents
        );
    }
    return presets;
}

void SimpUtil::convertPresetsImageProcessingToJsonArray(const std::vector<PresetImageProcessing>& presets, QJsonArray& jsonArray)
{
    for (const PresetImageProcessing& preset : presets)
    {
        QJsonObject jsonObject;
        jsonObject[SimpConstKey::INDEX] = preset.GetIndex();

        jsonObject[SimpConstKey::BRIGHTNESS_CONTRAST_ENABLE] = preset.GetBrightnessContrastEnable();
        jsonObject[SimpConstKey::BRIGHTNESS] = preset.GetBrightness();
        jsonObject[SimpConstKey::CONTRAST] = preset.GetContrast();

        jsonObject[SimpConstKey::STRESS_ENABLE] = preset.GetStressEnable();
        jsonObject[SimpConstKey::STRESS_RADIUS] = preset.GetStressRadius();
        jsonObject[SimpConstKey::STRESS_SAMPLES] = preset.GetStressSamples();
        jsonObject[SimpConstKey::STRESS_ITERATIONS] = preset.GetStressIterations();
        jsonObject[SimpConstKey::STRESS_ENHANCE_SHADOWS] = preset.GetStressEnhanceShadows();

        jsonObject[SimpConstKey::STRETCH_CONTRAST_ENABLE] = preset.GetStretchContrastEnable();
        jsonObject[SimpConstKey::STRETCH_CONTRAST_KEEP_COLORS] = preset.GetStretchContrastKeepColors();
        jsonObject[SimpConstKey::STRETCH_CONTRAST_NON_LINEAR_COMPONENTS] = preset.GetStretchContrastNonLinearComponents();

        jsonArray.append(jsonObject);
    }
}
