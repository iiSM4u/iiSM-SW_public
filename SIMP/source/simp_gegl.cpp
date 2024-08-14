#include "simp_gegl.h"

void SimpGEGL::Init(const QString& appDir)
{
    qputenv("BABL_PATH", (appDir + "/lib/babl-0.1").toUtf8());
    qputenv("GEGL_PATH", (appDir + "/lib/gegl-0.4").toUtf8());

    // gegl 초기화
    gegl_init(nullptr, nullptr);
}

void SimpGEGL::Close()
{
    // gegl 종료
    gegl_exit();
}

void SimpGEGL::Update_Image_Processing(QImage& source)
{
}


void SimpGEGL::Update_Contrast_Curve(QImage& source, const QVector<QPointF>& points)
{
}
