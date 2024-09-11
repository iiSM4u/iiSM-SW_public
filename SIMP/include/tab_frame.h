#pragma once

#include <QWidget>
#include <QFileSystemModel>
#include <QProgressDialog>

#include "dialog_image_processing.h"
#include "preset_contrast_curve.h"

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

    void btnCurveSetting_Click();
    void cbCurvePreset_SelectedIndexChanged(int index);

    void onFrameConvertingCanceled();
    void onFrameConvertingFinished(bool success, const QImage& frame);

private:
    Ui::TabFrame *ui;

    QProgressDialog *progressDialog;
    QFileSystemModel *filesystemModel;
    DialogImageProcessing *dialogImageProcessing;

    QString captureDir;
    QImage currentFrame;
    QModelIndex currentFrameIndex;

    int lastPresetIndex = -1;
    float zoomFactor = 1.0f;

    bool isUpdateContrastCurve = false;
    std::vector<PresetContrastCurve> presetsContrastCurve;
    QVector<QPointF> contrastCurvePoints;

    void ConnectUI();
    void InitUI();
    void ProcessingFrame();
    void UpdatePresetContrastCurve(const std::vector<PresetContrastCurve>& presets, const int index = 0);
    void UpdateContrastCurvePoints(const QVector<QPointF>& points);
};
