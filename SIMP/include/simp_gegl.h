#pragma once

#include <QImage>
#include <QMutex>

#include "simp_const_value.h"

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

    static void UpdateImageProcessing(
        QImage& source
        , bool isUpdateBrightnessContrast
        , bool isUpdateStress
        , bool isUpdateStretchContrast
        , double brightness = SimpConstValue::GEGL_BRIGHTNESS_DEFAULT
        , double contrast = SimpConstValue::GEGL_CONTRAST_DEFAULT
        , int stress_radius = SimpConstValue::GEGL_STRESS_RADIUS_DEFAULT
        , int stress_samples = SimpConstValue::GEGL_STRESS_SAMPLES_DEFAULT
        , int stress_iterations = SimpConstValue::GEGL_STRESS_ITERATIONS_DEFAULT
        , bool stress_enhance_shadows = SimpConstValue::GEGL_STRESS_ENHANCE_SHADOWS_DEFAULT
        , bool stretch_contrast_keep_colors = SimpConstValue::GEGL_STRETCH_CONTRAST_KEEP_COLORS_DEFAULT
        , bool stretch_contrast_perceptual = SimpConstValue::GEGL_STRETCH_CONTRAST_PERCEPTUAL_DEFAULT
    );

    static void UpdateContrastCurve(
        QImage& source
        , const QVector<QPointF>& points
        , const int valueMin = SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MIN
        , const int valueMax = SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MAX
        , const int samplingPoints = SimpConstValue::GEGL_CONTRAST_CURVE_SAMPLING_POINTS_DEFAULT
    );
};
