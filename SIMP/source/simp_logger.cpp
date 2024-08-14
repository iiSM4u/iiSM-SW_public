#include "simp_logger.h"
#include "simp_const_path.h"
#include "simp_const_format.h"

#include <QFile>
#include <QDateTime>

void SimpLogger::Log(const QString &message)
{
    QFile file(SimpConstPath::PATH_LOG);
    if (file.open(QIODevice::Append | QIODevice::Text))
    {
        QTextStream out(&file);
        out << QDateTime::currentDateTime().toString(SimpConstFormat::DATE_TIME) << " | " << message << "\n";
    }
}
