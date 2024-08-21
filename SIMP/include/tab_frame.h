#pragma once

#include <QWidget>
#include <QFileSystemModel>
#include <QProgressDialog>

namespace Ui {
class TabFrame;
}

class TabFrame : public QWidget
{
    Q_OBJECT

public:
    explicit TabFrame(QWidget *parent = nullptr);
    ~TabFrame();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void UpdateMousePosition(int x, int y, const QColor& color);
    void EnableUI(bool enable);

    void lvFrame_Click(const QModelIndex& index);
    void btnOpenDir_Click();
    void btnZoomIn_Click();
    void btnZoomOut_Click();
    void btnFrameProcessing_Click();
    void btnFrameSave_Click();

    void onFrameConvertingCanceled();
    void onFrameConvertingFinished(bool success, const QImage& frame);

private:
    Ui::TabFrame *ui;

    QProgressDialog *progressDialog;
    QFileSystemModel *filesystemModel;
    QString captureDir;
    QImage currentFrame;
    QModelIndex currentFrameIndex;

    int lastPresetIndex = -1;
    float zoomFactor = 1.0f;

    void ConnectUI();
    void InitUI();
    void ProcessingFrame(
        const int presetIndex
        , const bool isUpdateBrightnessContrast
        , const bool isUpdateStress
        , const bool isUpdateStretchContrast
        , const double brightness
        , const double contrast
        , const int stress_radius
        , const int stress_samples
        , const int stress_iterations
        , const bool stress_enhance_shadows
        , const bool stretch_contrast_keep_colors
        , const bool stretch_contrast_perceptual
        );
};
