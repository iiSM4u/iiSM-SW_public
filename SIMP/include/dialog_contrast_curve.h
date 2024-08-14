#pragma once

#include <QDialog>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#include <QMouseEvent>

#include "preset_contrast_curve.h"

namespace Ui {
class DialogContrastCurve;
}

class DialogContrastCurve : public QDialog
{
    Q_OBJECT

public:
    explicit DialogContrastCurve(QWidget *parent = nullptr);
    DialogContrastCurve(std::vector<PresetContrastCurve>& presets, const int presetIndex, const bool enable, QWidget *parent = nullptr);
    ~DialogContrastCurve();

    std::vector<PresetContrastCurve> getPresets() const;
    int getSelectedIndex() const;
    bool getEnable() const;

signals:
    void contrastCurveUpdated(const QVector<QPointF>& points);

private slots:
    void onOkClicked();
    void chkCurve_CheckedChanged(Qt::CheckState checkState);
    void cbPreset_SelectedIndexChanged(int index);

    void btnRemovePreset_Click();
    void btnSavePreset_Click();
    void btnResetPreset_Click();
    void btnDeletePoint_Click();

    void spinInput_ValueChanged(int value);
    void spinOutput_ValueChanged(int value);

    void EnableUI(bool enable);
    void UpdatePresetUI(const std::vector<PresetContrastCurve>& presets, const int index = -1);
    void UpdateSpinUI(int x, int y, bool enable);

    void handleChartClicked(const QPointF &point);
    void handlePointMoved(const QPointF &newPos);
    void handlePointMovingFinished();


private:
    Ui::DialogContrastCurve *ui;

    QChart *chart;
    QSplineSeries *splineSeries;
    QScatterSeries *scatterSeries, *highlightSeries;
    QVector<QPointF> qpoints;

    std::vector<PresetContrastCurve> presets;
    int highlightPointIndex = -1;
    bool isPresetChanged = false;

    void InitChart();
    void UpdateChart(const QVector<QPointF>& points, const int highlightIndex);
    void AddPoint(const int x, const int y);
    void SavePreset();
    void UpdatePreset();
};
