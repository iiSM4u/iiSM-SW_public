#pragma once

#include <QImage>

// C 라이브러리를 참조할 떄는 extern C로 묶는 것이 링킹 문제를 방지하는데 도움이 됨
extern "C" {
#include <glib.h>
#include <gegl.h>
#include <gegl-0.4/gegl.h>
#include <gegl-0.4/gegl-init.h>
#include <gegl-0.4/gegl-node.h>
#include <gegl-0.4/gegl-buffer.h>
}

class SimpGEGL
{
public:
    static void Init(const QString& appDir);
    static void Close();
    static void Update_Image_Processing(QImage& source);
    static void Update_Contrast_Curve(QImage& source, const QVector<QPointF>& points);
    //static void Update_contrast_curve_points(const QVector<QPointF>& points);
};
