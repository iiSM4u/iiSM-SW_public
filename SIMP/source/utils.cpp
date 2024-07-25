#include "utils.h"

#include <QJsonDocument>
#include <QDir>

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
