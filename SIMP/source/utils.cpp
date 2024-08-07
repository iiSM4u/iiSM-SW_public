#include "utils.h"
#include "constants.h"

#include <QPointF>
#include <QJsonDocument>
#include <QDir>
#include <opencv2/opencv.hpp>

double roundToDecimalPlaces(double value, int decimalPlaces) {
    double factor = std::pow(10.0, decimalPlaces);
    return std::round(value * factor) / factor;
}

bool loadJsonFile(const QString& filePath, QJsonArray& jsonArray)
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

bool saveJsonFile(const QString& filePath, const QJsonArray& jsonArray)
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

QString getVideoExtension(VideoFormatType type)
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


int getVideoFourcc(VideoFormatType type)
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

std::vector<preset_brightness_contrast> convertJsonToBrightnessContrastPresets(const QJsonArray& jsonArray)
{
    std::vector<preset_brightness_contrast> presets;
    for (const QJsonValue& value : jsonArray)
    {
        QJsonObject obj = value.toObject();
        int index = obj[KEY_INDEX].toInt();
        double brightness = obj[KEY_BRIGHTNESS].toDouble();
        double contrast = obj[KEY_CONTRAST].toDouble();
        presets.emplace_back(index, brightness, contrast);
    }
    return presets;
}

void convertBrightnessContrastPresetsToJsonArray(const std::vector<preset_brightness_contrast>& presets, QJsonArray& jsonArray)
{
    for (const preset_brightness_contrast& preset : presets)
    {
        QJsonObject jsonObject;
        jsonObject[KEY_INDEX] = preset.GetIndex();
        jsonObject[KEY_BRIGHTNESS] = preset.GetBrightness();
        jsonObject[KEY_CONTRAST] = preset.GetContrast();
        jsonArray.append(jsonObject);
    }
}

std::vector<preset_stress> convertJsonToStressPrestes(const QJsonArray& jsonArray)
{
    std::vector<preset_stress> presets;
    for (const QJsonValue& value : jsonArray)
    {
        QJsonObject obj = value.toObject();
        int index = obj[KEY_INDEX].toInt();
        int radius = obj[KEY_RADIUS].toInt();
        int samples = obj[KEY_SAMPLES].toInt();
        int iterations = obj[KEY_ITERATIONS].toInt();
        bool enhanceShadows = obj[KEY_ENHANCE_SHADOWS].toBool();
        presets.emplace_back(index, radius, samples, iterations, enhanceShadows);
    }
    return presets;
}

void convertStressPrestesToJsonArray(const std::vector<preset_stress>& presets, QJsonArray& jsonArray)
{
    for (const preset_stress& preset : presets)
    {
        QJsonObject jsonObject;
        jsonObject[KEY_INDEX] = preset.GetIndex();
        jsonObject[KEY_RADIUS] = preset.GetRadius();
        jsonObject[KEY_SAMPLES] = preset.GetSamples();
        jsonObject[KEY_ITERATIONS] = preset.GetIterations();
        jsonObject[KEY_ENHANCE_SHADOWS] = preset.GetEnhanceShadows();
        jsonArray.append(jsonObject);
    }
}

std::vector<preset_contrast_curve> convertJsonToPresetsImageCurve(const QJsonArray& jsonArray)
{
    std::vector<preset_contrast_curve> presets;
    for (const QJsonValue& value : jsonArray)
    {
        QJsonObject obj = value.toObject();
        int index = obj[KEY_INDEX].toInt();

        QVector<QPointF> points;
        for (const QJsonValue& pointValue : obj[KEY_POINTS].toArray())
        {
            QJsonObject point = pointValue.toObject();
            int x = point[KEY_POS_X].toInt();
            int y = point[KEY_POS_Y].toInt();

            points.emplace_back(x, y);
        }

        presets.emplace_back(index, points);
    }
    return presets;
}

void convertPresetsImageCurveToJsonArray(const std::vector<preset_contrast_curve>& presets, QJsonArray& jsonArray)
{
    for (const preset_contrast_curve& preset : presets)
    {
        QJsonArray pointsArray;

        for (const QPointF& point : preset.GetPoints())
        {
            QJsonObject pointObj;
            pointObj[KEY_POS_X] = point.x();
            pointObj[KEY_POS_Y] = point.y();

            pointsArray.append(pointObj);
        }

        QJsonObject jsonObject;
        jsonObject[KEY_INDEX] = preset.GetIndex();
        jsonObject[KEY_POINTS] = pointsArray;

        jsonArray.append(jsonObject);
    }
}

