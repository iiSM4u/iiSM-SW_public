#include "simp_gegl.h"
#include <QMutex>

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

void SimpGEGL::UpdateImageProcessing(
    QImage& source
    , bool isUpdateBrightnessContrast
    , bool isUpdateStress
    , bool isUpdateStretchContrast
    , double brightness
    , double contrast
    , int stress_radius
    , int stress_samples
    , int stress_iterations
    , bool stress_enhance_shadows
    , bool stretch_contrast_keep_colors
    , bool stretch_contrast_perceptual
)
{
    // Create GEGL buffers
    GeglBuffer* input_buffer = gegl_buffer_new(GEGL_RECTANGLE(0, 0, source.width(), source.height()), babl_format("RGBA u8"));
    GeglBuffer* output_buffer = gegl_buffer_new(GEGL_RECTANGLE(0, 0, source.width(), source.height()), babl_format("RGBA u8"));

    // Set input buffer data
    gegl_buffer_set(input_buffer, nullptr, 0, babl_format("RGBA u8"), source.bits(), GEGL_AUTO_ROWSTRIDE);

    // Create GEGL graph
    GeglNode* graph = gegl_node_new();
    GeglNode* input = gegl_node_new_child(graph, "operation", "gegl:buffer-source", "buffer", input_buffer, nullptr);
    GeglNode* output = gegl_node_new_child(graph, "operation", "gegl:write-buffer", "buffer", output_buffer, nullptr);

    GeglNode* brightness_contrast = nullptr;
    GeglNode* stress = nullptr;
    GeglNode* stretch_contrast = nullptr;

    // gegl_node_set()은 gegl_node_link()를 하기 전에 마쳐야 한다.
    if (isUpdateBrightnessContrast)
    {
        brightness_contrast = gegl_node_new_child(graph, "operation", "gegl:brightness-contrast", nullptr);
        gegl_node_set(brightness_contrast, "brightness", brightness, "contrast", contrast, nullptr);
    }

    if (isUpdateStress)
    {
        stress = gegl_node_new_child(graph, "operation", "gegl:stress", nullptr);
        gegl_node_set(stress, "radius", stress_radius, "samples", stress_samples, "iterations", stress_iterations, "enhance-shadows", stress_enhance_shadows, nullptr);
    }

    if (isUpdateStretchContrast)
    {
        stretch_contrast = gegl_node_new_child(graph, "operation", "gegl:stretch-contrast", nullptr);
        gegl_node_set(stretch_contrast, "keep-colors", stretch_contrast_keep_colors, "perceptual", stretch_contrast_perceptual, nullptr);
    }

    // Link nodes
    GeglNode* last_node = input;
    if (brightness_contrast)
    {
        gegl_node_link(last_node, brightness_contrast);
        last_node = brightness_contrast;
    }
    if (stress)
    {
        gegl_node_link(last_node, stress);
        last_node = stress;
    }
    if (stretch_contrast)
    {
        gegl_node_link(last_node, stretch_contrast);
        last_node = stretch_contrast;
    }
    gegl_node_link(last_node, output);

    gegl_node_process(output);

    // Get output buffer data
    gegl_buffer_get(output_buffer, nullptr, 1.0, babl_format("RGBA u8"), source.bits(), GEGL_AUTO_ROWSTRIDE, GEGL_ABYSS_NONE);

    g_object_unref(input_buffer);
    g_object_unref(output_buffer);
    g_object_unref(graph);
}

void SimpGEGL::UpdateContrastCurve(
    QImage& source
    , const QVector<QPointF>& points
    , const int valueMin
    , const int valueMax
    , const int samplingPoints
)
{
    if (points.size() > 0)
    {
        GeglCurve *contrast_curve_points = gegl_curve_new(valueMin, valueMax);

        for (const QPointF& point : points)
        {
            gegl_curve_add_point(contrast_curve_points, point.x(), point.y());
        }

        // Create GEGL buffers
        GeglBuffer* input_buffer = gegl_buffer_new(GEGL_RECTANGLE(0, 0, source.width(), source.height()), babl_format("RGBA u8"));
        GeglBuffer* output_buffer = gegl_buffer_new(GEGL_RECTANGLE(0, 0, source.width(), source.height()), babl_format("RGBA u8"));

        // Set input buffer data
        gegl_buffer_set(input_buffer, nullptr, 0, babl_format("RGBA u8"), source.bits(), GEGL_AUTO_ROWSTRIDE);

        // Create GEGL graph
        GeglNode* graph = gegl_node_new();
        GeglNode* input = gegl_node_new_child(graph, "operation", "gegl:buffer-source", "buffer", input_buffer, nullptr);
        GeglNode* output = gegl_node_new_child(graph, "operation", "gegl:write-buffer", "buffer", output_buffer, nullptr);

        GeglNode* contrast_curve = gegl_node_new_child(graph, "operation", "gegl:contrast-curve", nullptr);
        gegl_node_set(contrast_curve, "curve", contrast_curve_points, "sampling-points", samplingPoints, nullptr);

        // Link nodes
        gegl_node_link(input, contrast_curve);
        gegl_node_link(contrast_curve, output);

        gegl_node_process(output);

        // Get output buffer data
        gegl_buffer_get(output_buffer, nullptr, 1.0, babl_format("RGBA u8"), source.bits(), GEGL_AUTO_ROWSTRIDE, GEGL_ABYSS_NONE);

        g_object_unref(contrast_curve_points);
        g_object_unref(input_buffer);
        g_object_unref(output_buffer);
        g_object_unref(graph);
    }
}
