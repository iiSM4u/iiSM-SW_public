#include "video_converter.h"
#include "simp_gegl.h"

#include <QImage>

VideoConverter::VideoConverter(
    const std::vector<QImage>& videoFrames
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
   , QObject *parent
)
    : QThread(parent)
    , isUpdateBrightnessContrast(isUpdateBrightnessContrast)
    , isUpdateStress(isUpdateStress)
    , isUpdateStretchContrast(isUpdateStretchContrast)
    , brightness(brightness)
    , contrast(contrast)
    , stress_radius(stress_radius)
    , stress_samples(stress_samples)
    , stress_iterations(stress_iterations)
    , stress_enhance_shadows(stress_enhance_shadows)
    , stretch_contrast_keep_colors(stretch_contrast_keep_colors)
    , stretch_contrast_perceptual(stretch_contrast_perceptual)
{
    this->sources = videoFrames;
}

void VideoConverter::run()
{
    this->results.clear();

    int total = this->sources.size();

    for (int i = 0; i < total; i++)
    {
        // gegl에 넣기 위해 rgba8888로 변경
        QImage source = this->sources[i].convertToFormat(QImage::Format_RGBA8888);

        SimpGEGL::UpdateImageProcessing(
            source
            , /*isUpdateBrightnessContrast*/this->isUpdateBrightnessContrast
            , /*isUpdateStress*/this->isUpdateStress
            , /*isUpdateStretchContrast*/this->isUpdateStretchContrast
            , /*brightness*/this->brightness
            , /*contrast*/this->contrast
            , /*stress_radius*/this->stress_radius
            , /*stress_samples*/this->stress_samples
            , /*stress_iterations*/this->stress_iterations
            , /*stress_enhance_shadows*/this->stress_enhance_shadows
            , /*stretch_contrast_keep_colors*/this->stretch_contrast_keep_colors
            , /*stretch_contrast_perceptual*/this->stretch_contrast_perceptual
        );

        // ui에 띄우기 위해 다시 rgb888로 돌림
        this->results.emplace_back(source.convertToFormat(QImage::Format_RGB888));

        emit progress(i, total);
    }

    emit finished(true, this->results);
}
