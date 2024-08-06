#include "dialog_contrast_curve.h"
#include "ui_dialog_contrast_curve.h"
#include "constants.h"
#include "utils.h"

dialog_contrast_curve::dialog_contrast_curve(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialog_contrast_curve)
{
    ui->setupUi(this);
}

dialog_contrast_curve::dialog_contrast_curve(std::vector<preset_contrast_curve>& presets, const int presetIndex, const bool enable, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialog_contrast_curve)
    , chart(new QChart())
    , splineSeries(new QSplineSeries())
    , scatterSeries(new QScatterSeries())
    , selectedPointIndex(-1)
    , presets(presets)
{
    ui->setupUi(this);

    connect(ui->chkCurve, &QCheckBox::checkStateChanged, this, &dialog_contrast_curve::chkCurve_CheckedChanged);
    connect(ui->cbPresets, &QComboBox::currentIndexChanged, this, &dialog_contrast_curve::cbPreset_SelectedIndexChanged);

    connect(ui->btnRemovePreset, &QPushButton::clicked, this, &dialog_contrast_curve::btnRemovePreset_Click);
    connect(ui->btnSavePreset, &QPushButton::clicked, this, &dialog_contrast_curve::btnSavePreset_Click);
    connect(ui->btnResetPreset, &QPushButton::clicked, this, &dialog_contrast_curve::btnResetPreset_Click);

    connect(ui->gvChartCurve, &CustomChartView::chartClicked, this, &dialog_contrast_curve::handleChartClicked);
    connect(ui->gvChartCurve, &CustomChartView::pointMoved, this, &dialog_contrast_curve::handlePointMoved);
    connect(ui->gvChartCurve, &CustomChartView::pointMovingFinishied, this, &dialog_contrast_curve::handlePointMovingFinished);

    dialog_contrast_curve::InitChart();

    ui->chkCurve->setCheckState(enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    dialog_contrast_curve::EnableUI(enable);

    if (presetIndex > -1)
    {
        dialog_contrast_curve::UpdatePresetUI(this->presets, presetIndex);
    }
    else
    {
        this->qpoints.clear();
        this->qpoints.append(QPointF(GEGL_CONTRAST_CURVE_VALUE_MIN, GEGL_CONTRAST_CURVE_VALUE_MIN));
        this->qpoints.append(QPointF(GEGL_CONTRAST_CURVE_VALUE_MAX, GEGL_CONTRAST_CURVE_VALUE_MAX));
        dialog_contrast_curve::UpdateChart(this->qpoints);
    }
}


dialog_contrast_curve::~dialog_contrast_curve()
{
    delete ui;
}

std::vector<preset_contrast_curve> dialog_contrast_curve::getPresets() const
{
    return this->presets;
}

int dialog_contrast_curve::getSelectedIndex() const
{
    return ui->cbPresets->currentIndex();
}

bool dialog_contrast_curve::getEnable() const
{
    return ui->chkCurve->isChecked();
}

void dialog_contrast_curve::chkCurve_CheckedChanged(Qt::CheckState checkState)
{
    dialog_contrast_curve::EnableUI(checkState == Qt::CheckState::Checked);
}

void dialog_contrast_curve::cbPreset_SelectedIndexChanged(int index)
{
    if (index > -1)
    {
        this->qpoints = convertCurvePointsToQPointf(this->presets[index].GetPoints());
    }
    else
    {
        this->qpoints.clear();
        this->qpoints.append(QPointF(GEGL_CONTRAST_CURVE_VALUE_MIN, GEGL_CONTRAST_CURVE_VALUE_MIN));
        this->qpoints.append(QPointF(GEGL_CONTRAST_CURVE_VALUE_MAX, GEGL_CONTRAST_CURVE_VALUE_MAX));
    }

    dialog_contrast_curve::UpdateChart(this->qpoints);
}

void dialog_contrast_curve::btnRemovePreset_Click()
{
    if (ui->cbPresets->currentIndex() > -1)
    {
        this->presets.erase(this->presets.begin() + ui->cbPresets->currentIndex());
        dialog_contrast_curve::UpdatePresetUI(this->presets);
    }
}

void dialog_contrast_curve::btnSavePreset_Click()
{
    bool ok;
    std::vector<curve_point> points = convertQPointfToCurvePoints(this->qpoints);
    int index = this->presets.size();
    this->presets.emplace_back(index, points);

    dialog_contrast_curve::UpdatePresetUI(this->presets, index);
}

void dialog_contrast_curve::btnResetPreset_Click()
{
    ui->cbPresets->setCurrentIndex(-1);
}

void dialog_contrast_curve::handleChartClicked(const QPointF &point)
{
    if (point.x() >= GEGL_CONTRAST_CURVE_VALUE_MIN && point.x() <= GEGL_CONTRAST_CURVE_VALUE_MAX
        && point.y() >= GEGL_CONTRAST_CURVE_VALUE_MIN && point.y() <= GEGL_CONTRAST_CURVE_VALUE_MAX)
    {
        // Check if a point is selected for dragging
        for (int i = 0; i < this->qpoints.size(); ++i)
        {
            if (qAbs(this->qpoints[i].x() - point.x()) < 5 && qAbs(this->qpoints[i].y() - point.y()) < 5)
            {
                selectedPointIndex = i;
                return;
            }
        }

        // Add a new point
        //AddPoint(point);

        this->qpoints.append(point);
        std::sort(this->qpoints.begin(), this->qpoints.end(), [](const QPointF &a, const QPointF &b) { return a.x() < b.x(); });

        UpdateChart(this->qpoints);
    }
}

void dialog_contrast_curve::handlePointMoved(const int selectedPointIndex, const QPointF &newPos)
{
    this->qpoints[selectedPointIndex] = newPos;
    // 이동 중에는 sort를 하지 않는다.
    UpdateChart(this->qpoints);
}

void dialog_contrast_curve::handlePointMovingFinished()
{
    // 이동이 끝나면 sort
    std::sort(this->qpoints.begin(), this->qpoints.end(), [](const QPointF &a, const QPointF &b) { return a.x() < b.x(); });
    UpdateChart(this->qpoints);
}

void dialog_contrast_curve::EnableUI(bool enable)
{
    ui->cbPresets->setEnabled(enable);
    ui->btnRemovePreset->setEnabled(enable);
    ui->btnSavePreset->setEnabled(enable);
    ui->btnResetPreset->setEnabled(enable);
    ui->gvChartCurve->setEnabled(enable);
}

void dialog_contrast_curve::UpdatePresetUI(const std::vector<preset_contrast_curve>& presets, const int index)
{
    ui->cbPresets->clear();

    if (presets.size() > 0)
    {
        for (const preset_contrast_curve& preset : presets)
        {
            QString message = QString("index: %1, points: %2")
                                  .arg(preset.GetIndex()) // 'f' for floating point, 2 decimal places
                                  .arg(preset.GetPoints().size());
            ui->cbPresets->addItem(message);
        }
    }

    ui->cbPresets->setCurrentIndex(index);
}

void dialog_contrast_curve::InitChart()
{
    // Setup the chart
    this->chart->addSeries(this->splineSeries);
    this->chart->addSeries(this->scatterSeries);
    this->chart->legend()->hide();

    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();

    // Setup the axes
    axisX->setRange(GEGL_CONTRAST_CURVE_VALUE_MIN, GEGL_CONTRAST_CURVE_VALUE_MAX);
    axisY->setRange(GEGL_CONTRAST_CURVE_VALUE_MIN, GEGL_CONTRAST_CURVE_VALUE_MAX);

    this->chart->addAxis(axisX, Qt::AlignBottom);
    this->chart->addAxis(axisY, Qt::AlignLeft);

    this->splineSeries->attachAxis(axisX);
    this->splineSeries->attachAxis(axisY);
    this->splineSeries->setPointLabelsClipping(true);

    this->scatterSeries->attachAxis(axisX);
    this->scatterSeries->attachAxis(axisY);    

    // Set the chart in the QChartView
    ui->gvChartCurve->setChart(this->chart);
    ui->gvChartCurve->setRenderHint(QPainter::Antialiasing);
}

void dialog_contrast_curve::UpdateChart(const QVector<QPointF>& points)
{
    this->splineSeries->clear();
    this->splineSeries->append(points);

    this->scatterSeries->clear();
    this->scatterSeries->append(points);
}
