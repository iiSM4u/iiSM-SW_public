#pragma once

#include <QFile>
#include <QJsonArray>
#include "video_format_type.h"

double roundToDecimalPlaces(double value, int decimalPlaces);
bool loadJsonFile(const QString& path, QJsonArray& jsonArray);
bool saveJsonFile(const QString& filePath, const QJsonArray& jsonArray);

QString getVideoExtension(VideoFormatType type);
int getVideoFourcc(VideoFormatType type);
