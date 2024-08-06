#include "dialog_contrast_curve.h"
#include "ui_dialog_contrast_curve.h"
#include "dialog_curve_point.h"
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
    , highlightSeries(new QScatterSeries())
    , presets(presets)
{
    ui->setupUi(this);

    connect(ui->chkCurve, &QCheckBox::checkStateChanged, this, &dialog_contrast_curve::chkCurve_CheckedChanged);
    connect(ui->cbPresets, &QComboBox::currentIndexChanged, this, &dialog_contrast_curve::cbPreset_SelectedIndexChanged);

    connect(ui->btnRemovePreset, &QPushButton::clicked, this, &dialog_contrast_curve::btnRemovePreset_Click);
    connect(ui->btnSavePreset, &QPushButton::clicked, this, &dialog_contrast_curve::btnSavePreset_Click);
    connect(ui->btnResetPreset, &QPushButton::clicked, this, &dialog_contrast_curve::btnResetPreset_Click);

    connect(ui->spinInput, &QSpinBox::valueChanged, this, &dialog_contrast_curve::spinInput_ValueChanged);
    connect(ui->spinOutput, &QSpinBox::valueChanged, this, &dialog_contrast_curve::spinOutput_ValueChanged);

    connect(ui->gvChartCurve, &CustomChartView::chartClicked, this, &dialog_contrast_curve::handleChartClicked);
    connect(ui->gvChartCurve, &CustomChartView::pointMoved, this, &dialog_contrast_curve::handlePointMoved);
    connect(ui->gvChartCurve, &CustomChartView::pointMovingFinishied, this, &dialog_contrast_curve::handlePointMovingFinished);

    dialog_contrast_curve::InitChart();

    ui->chkCurve->setCheckState(enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    dialog_contrast_curve::EnableUI(enable);

    this->highlightPointIndex = -1;
    dialog_contrast_curve::UpdateSpinUI(0, 0, false);

    if (presetIndex > -1)
    {
        dialog_contrast_curve::UpdatePresetUI(this->presets, presetIndex);
    }
    else
    {
        this->qpoints.clear();
        this->qpoints.append(QPointF(GEGL_CONTRAST_CURVE_VALUE_MIN, GEGL_CONTRAST_CURVE_VALUE_MIN));
        this->qpoints.append(QPointF(GEGL_CONTRAST_CURVE_VALUE_MAX, GEGL_CONTRAST_CURVE_VALUE_MAX));

        dialog_contrast_curve::UpdateChart(this->qpoints, this->highlightPointIndex);
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

    ui->spinInput->setEnabled(false);
    ui->spinOutput->setEnabled(false);
    this->highlightPointIndex = -1;
    dialog_contrast_curve::UpdateChart(this->qpoints, this->highlightPointIndex);
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

void dialog_contrast_curve::spinInput_ValueChanged(int value)
{
    if (this->highlightPointIndex > -1)
    {
        QPointF point = this->qpoints[this->highlightPointIndex];
        this->qpoints[this->highlightPointIndex] = QPointF(value, point.y());
        dialog_contrast_curve::UpdateChart(this->qpoints, this->highlightPointIndex);
    }
}

void dialog_contrast_curve::spinOutput_ValueChanged(int value)
{
    if (this->highlightPointIndex > -1)
    {
        QPointF point = this->qpoints[this->highlightPointIndex];
        this->qpoints[this->highlightPointIndex] = QPointF(point.x(), value);
        dialog_contrast_curve::UpdateChart(this->qpoints, this->highlightPointIndex);
    }
}

void dialog_contrast_curve::handleChartClicked(const QPointF &point)
{
    // 일단 초기화
    this->highlightPointIndex = -1;
    ui->spinInput->setEnabled(false);
    ui->spinOutput->setEnabled(false);

    double x = point.x();
    double y = point.y();

    if (x >= GEGL_CONTRAST_CURVE_VALUE_MIN && x <= GEGL_CONTRAST_CURVE_VALUE_MAX
        && y >= GEGL_CONTRAST_CURVE_VALUE_MIN && y <= GEGL_CONTRAST_CURVE_VALUE_MAX)
    {
        // custom chart에서 들어오는 index가 여기의 vector index와 다르기 때문에 여기서 새로 찾아야 함.
        for (int i = 0; i < this->qpoints.size(); i++)
        {
            if (qAbs(this->qpoints[i].x() - x) < MARGIN_CHART_CLICK && qAbs(this->qpoints[i].y() - y) < MARGIN_CHART_CLICK)
            {
                this->highlightPointIndex = i;
                dialog_contrast_curve::UpdateSpinUI(this->qpoints[i].x(), this->qpoints[i].y(), true);
                return;
            }
        }

        // 선택된게 없었으면 add point
        dialog_contrast_curve::AddPoint(x, y);
    }
}

void dialog_contrast_curve::handlePointMoved(const QPointF &newPos)
{
    if (this->highlightPointIndex > -1)
    {
        this->qpoints[this->highlightPointIndex] = newPos;
        // 이동 중에는 sort를 하지 않는다.

        dialog_contrast_curve::UpdateSpinUI(newPos.x(), newPos.y(), true);
        dialog_contrast_curve::UpdateChart(this->qpoints, this->highlightPointIndex);
    }
}

void dialog_contrast_curve::handlePointMovingFinished()
{
    // 이동이 끝나면 sort
    std::sort(this->qpoints.begin(), this->qpoints.end(), [](const QPointF &a, const QPointF &b) { return a.x() < b.x(); });
    dialog_contrast_curve::UpdateChart(this->qpoints, this->highlightPointIndex);
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

void dialog_contrast_curve::UpdateSpinUI(int x, int y, bool enable)
{
    {
        const QSignalBlocker blocker(ui->spinInput);
        ui->spinInput->setValue(x);
        ui->spinOutput->setValue(y);
        ui->spinInput->setEnabled(enable);
        ui->spinOutput->setEnabled(enable);
    }
}

void dialog_contrast_curve::InitChart()
{
    // Setup the chart
    this->chart->addSeries(this->splineSeries);
    this->chart->addSeries(this->scatterSeries);
    this->chart->addSeries(this->highlightSeries);
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

    this->highlightSeries->attachAxis(axisX);
    this->highlightSeries->attachAxis(axisY);

    // Set the chart in the QChartView
    ui->gvChartCurve->setChart(this->chart);
    ui->gvChartCurve->setRenderHint(QPainter::Antialiasing);
}

void dialog_contrast_curve::UpdateChart(const QVector<QPointF>& points, const int highlightIndex)
{
    QVector<QPointF> normals;
    QVector<QPointF> highlights;

    if (highlightIndex > -1)
    {
        for (int i = 0; i < points.size(); i++)
        {
            if (i != highlightIndex)
            {
                normals.emplaceBack(points[i]);
            }
            else
            {
                highlights.emplaceBack(points[i]);
            }
        }
    }
    else
    {
        normals = points;
    }

    this->splineSeries->clear();
    this->splineSeries->append(points);

    this->scatterSeries->clear();
    this->scatterSeries->append(normals);

    this->highlightSeries->clear();
    this->highlightSeries->append(highlights);
}

void dialog_contrast_curve::AddPoint(const int x, const int y)
{
    // select point가 없었으면 point 추가
    dialog_curve_point dialog(this->qpoints, x, y, this);

    if (dialog.exec() == QDialog::Accepted)
    {
        // 새로 추가한 후에 정렬한다.
        this->qpoints.emplaceBack(dialog.getInputValue(), dialog.getOutputValue());
        std::sort(this->qpoints.begin(), this->qpoints.end(), [](const QPointF &a, const QPointF &b) { return a.x() < b.x(); });

        this->highlightPointIndex = -1;
        dialog_contrast_curve::UpdateChart(this->qpoints, this->highlightPointIndex);
        dialog_contrast_curve::UpdateSpinUI(0, 0, false);
    }
}
