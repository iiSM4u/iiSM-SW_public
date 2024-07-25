#pragma once

#include <QFile>
#include <QJsonArray>

double roundToDecimalPlaces(double value, int decimalPlaces);
bool loadJsonFile(const QString& path, QJsonArray& jsonArray);
bool saveJsonFile(const QString& filePath, const QJsonArray& jsonArray);
