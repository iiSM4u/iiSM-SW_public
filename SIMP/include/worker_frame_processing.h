#pragma once

#include <QThread>

class WorkerFrameProcessing : public QThread
{
    Q_OBJECT

public:
    WorkerFrameProcessing(
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
        , QObject *parent = nullptr
    );

    void run() override;

signals:
    void finished(bool success, const QImage& frame);
    void cancelled();

private:
    QString filePath;
    bool isUpdateBrightnessContrast, isUpdateStress, isUpdateStretchContrast,
        stress_enhance_shadows, stretch_contrast_keep_colors, stretch_contrast_perceptual;
    int stress_radius, stress_samples, stress_iterations;
    double brightness, contrast;
};
