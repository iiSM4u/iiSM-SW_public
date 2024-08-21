#include "worker_frame_processing.h"
#include "simp_gegl.h"

#include <QImage>
#include <opencv2/opencv.hpp>

WorkerFrameProcessing::WorkerFrameProcessing(
    const QString& filePath
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
    , filePath(filePath)
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
{ }

void WorkerFrameProcessing::run()
{
    QImage img(this->filePath);
    QImage source = img.convertToFormat(QImage::Format_RGBA8888);

    // 긴 작업이 시작되기 전에 스레드가 중단 요청을 받았는지 확인
    if (QThread::currentThread()->isInterruptionRequested()) {
        emit cancelled();
        return;
    }

    SimpGEGL::UpdateImageProcessing(
        /*source*/source
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

    if (QThread::currentThread()->isInterruptionRequested()) {
        emit cancelled();
        return;
    }

    // 다시 rgb888로 돌림
    emit finished(true, source.convertToFormat(QImage::Format_RGB888));
}
