#pragma once

#include <QDialog>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#include <QMouseEvent>

#include "preset_contrast_curve.h"

namespace Ui {
class dialog_contrast_curve;
}

class dialog_contrast_curve : public QDialog
{
    Q_OBJECT

public:
    explicit dialog_contrast_curve(QWidget *parent = nullptr);
    dialog_contrast_curve(std::vector<preset_contrast_curve>& presets, const int presetIndex, const bool enable, QWidget *parent = nullptr);
    ~dialog_contrast_curve();

    std::vector<preset_contrast_curve> getPresets() const;
    int getSelectedIndex() const;
    bool getEnable() const;

private slots:
    void chkCurve_CheckedChanged(Qt::CheckState checkState);
    void cbPreset_SelectedIndexChanged(int index);
    void btnSavePreset_Click();
    void btnClear_Click();

    void EnableUI(bool enable);
    void UpdatePresetUI(const std::vector<preset_contrast_curve>& presets, const int index = -1);
    void handleChartClicked(const QPointF &point);
    void handlePointMoved(const int selectedPointIndex, const QPointF &newPos);
    void handlePointMovingFinished();

private:
    Ui::dialog_contrast_curve *ui;

    QChart *chart;
    QSplineSeries *splineSeries;
    QScatterSeries *scatterSeries;
    QVector<QPointF> qpoints;

    std::vector<preset_contrast_curve> presets;
    int selectedPointIndex;

    void InitChart();
    void UpdateChart(const QVector<QPointF>& points);
};
