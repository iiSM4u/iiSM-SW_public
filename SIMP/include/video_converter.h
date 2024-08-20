#pragma once

#include <QThread>

class VideoConverter : public QThread
{
    Q_OBJECT

public:
    VideoConverter(
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
        , QObject *parent = nullptr
    );

    void run() override;

signals:
    void progress(int current, int total);
    void finished(bool success, const std::vector<QImage>& frames);

private:
    std::vector<QImage> sources, results;
    bool isUpdateBrightnessContrast, isUpdateStress, isUpdateStretchContrast,
        stress_enhance_shadows, stretch_contrast_keep_colors, stretch_contrast_perceptual;
    int stress_radius, stress_samples, stress_iterations;
    double brightness, contrast;
};
