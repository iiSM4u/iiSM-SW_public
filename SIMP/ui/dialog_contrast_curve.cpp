#include "dialog_contrast_curve.h"
#include "ui_dialog_contrast_curve.h"
#include "dialog_curve_point.h"
#include "simp_const_value.h"

DialogContrastCurve::DialogContrastCurve(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogContrastCurve)
{
    ui->setupUi(this);
}

DialogContrastCurve::DialogContrastCurve(std::vector<PresetContrastCurve>& presets, const int presetIndex, const bool enable, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogContrastCurve)
    , chart(new QChart())
    , splineSeries(new QSplineSeries())
    , scatterSeries(new QScatterSeries())
    , highlightSeries(new QScatterSeries())
    , presets(presets)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DialogContrastCurve::onOkClicked);

    connect(ui->chkCurve, &QCheckBox::checkStateChanged, this, &DialogContrastCurve::chkCurve_CheckedChanged);
    connect(ui->cbPresets, &QComboBox::currentIndexChanged, this, &DialogContrastCurve::cbPreset_SelectedIndexChanged);

    connect(ui->btnRemovePreset, &QPushButton::clicked, this, &DialogContrastCurve::btnRemovePreset_Click);
    connect(ui->btnSavePreset, &QPushButton::clicked, this, &DialogContrastCurve::btnSavePreset_Click);
    connect(ui->btnResetPreset, &QPushButton::clicked, this, &DialogContrastCurve::btnResetPreset_Click);
    connect(ui->btnDeletePoint, &QPushButton::clicked, this, &DialogContrastCurve::btnDeletePoint_Click);

    connect(ui->spinInput, &QSpinBox::valueChanged, this, &DialogContrastCurve::spinInput_ValueChanged);
    connect(ui->spinOutput, &QSpinBox::valueChanged, this, &DialogContrastCurve::spinOutput_ValueChanged);

    connect(ui->gvChartCurve, &CustomChartView::chartClicked, this, &DialogContrastCurve::handleChartClicked);
    connect(ui->gvChartCurve, &CustomChartView::pointMoved, this, &DialogContrastCurve::handlePointMoved);
    connect(ui->gvChartCurve, &CustomChartView::pointMovingFinishied, this, &DialogContrastCurve::handlePointMovingFinished);

    DialogContrastCurve::InitChart();

    ui->chkCurve->setCheckState(enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    DialogContrastCurve::EnableUI(enable);

    this->highlightPointIndex = -1;
    DialogContrastCurve::UpdateSpinUI(0, 0, false);

    this->qpoints.clear();
    this->qpoints.append(QPointF(SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MIN, SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MIN));
    this->qpoints.append(QPointF(SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MAX, SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MAX));

    DialogContrastCurve::UpdateChart(this->qpoints, this->highlightPointIndex);
    DialogContrastCurve::UpdatePresetUI(this->presets, presetIndex);
}


DialogContrastCurve::~DialogContrastCurve()
{
    delete ui;
}

std::vector<PresetContrastCurve> DialogContrastCurve::getPresets() const
{
    return this->presets;
}

int DialogContrastCurve::getSelectedIndex() const
{
    return ui->cbPresets->currentIndex();
}

bool DialogContrastCurve::getEnable() const
{
    return ui->chkCurve->isChecked();
}

void DialogContrastCurve::onOkClicked()
{
    // 저장안된 변경이 있었으면 업데이트 한다.
    if (this->isPresetChanged)
    {
        int index = ui->cbPresets->currentIndex();

        // update
        if (index > -1)
        {
            index = this->presets[index].GetIndex();
            this->presets[index] = PresetContrastCurve(index, this->qpoints);
        }
        // add
        else
        {
            index = this->presets.size() > 0 ? this->presets[this->presets.size() - 1].GetIndex() + 1 : 0;
            this->presets.emplace_back(index, this->qpoints);
        }
    }

    accept();
}

void DialogContrastCurve::chkCurve_CheckedChanged(Qt::CheckState checkState)
{
    DialogContrastCurve::EnableUI(checkState == Qt::CheckState::Checked);
}

void DialogContrastCurve::cbPreset_SelectedIndexChanged(int index)
{
    if (index > -1)
    {
        this->qpoints = this->presets[index].GetPoints();
    }
    else
    {
        this->qpoints.clear();
        this->qpoints.append(QPointF(SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MIN, SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MIN));
        this->qpoints.append(QPointF(SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MAX, SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MAX));
    }

    this->highlightPointIndex = -1;
    DialogContrastCurve::UpdateChart(this->qpoints, this->highlightPointIndex);
    DialogContrastCurve::UpdateSpinUI(0, 0, false);
    this->isPresetChanged = false;

    emit contrastCurveUpdated(this->qpoints);
}

void DialogContrastCurve::btnRemovePreset_Click()
{
    if (ui->cbPresets->currentIndex() > -1)
    {
        this->presets.erase(this->presets.begin() + ui->cbPresets->currentIndex());
        DialogContrastCurve::UpdatePresetUI(this->presets);
        this->isPresetChanged = false;
    }
}

void DialogContrastCurve::btnSavePreset_Click()
{
    int index = ui->cbPresets->currentIndex();

    // update
    if (index > -1)
    {
        index = this->presets[index].GetIndex();
        this->presets[index] = PresetContrastCurve(index, this->qpoints);
    }
    // add
    else
    {
        index = this->presets[this->presets.size() - 1].GetIndex() + 1;
        this->presets.emplace_back(index, this->qpoints);
    }

    DialogContrastCurve::UpdatePresetUI(this->presets, index);
    this->isPresetChanged = false;
}

void DialogContrastCurve::btnResetPreset_Click()
{
    ui->cbPresets->setCurrentIndex(-1);
}

void DialogContrastCurve::btnDeletePoint_Click()
{
    if (this->highlightPointIndex > -1)
    {
        this->qpoints.erase(this->qpoints.begin() + this->highlightPointIndex);
        this->highlightPointIndex = -1;
        DialogContrastCurve::UpdateChart(this->qpoints, this->highlightPointIndex);
        this->isPresetChanged = true;

        emit contrastCurveUpdated(this->qpoints);
    }
}

void DialogContrastCurve::spinInput_ValueChanged(int value)
{
    if (this->highlightPointIndex > -1)
    {
        this->qpoints[this->highlightPointIndex].setX(value);
        DialogContrastCurve::UpdateChart(this->qpoints, this->highlightPointIndex);
        this->isPresetChanged = true;

        emit contrastCurveUpdated(this->qpoints);
    }
}

void DialogContrastCurve::spinOutput_ValueChanged(int value)
{
    if (this->highlightPointIndex > -1)
    {
        this->qpoints[this->highlightPointIndex].setY(value);
        DialogContrastCurve::UpdateChart(this->qpoints, this->highlightPointIndex);
        this->isPresetChanged = true;

        emit contrastCurveUpdated(this->qpoints);
    }
}

void DialogContrastCurve::handleChartClicked(const QPointF &point)
{
    double x = point.x();
    double y = point.y();

    if (x >= SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MIN && x <= SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MAX
        && y >= SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MIN && y <= SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MAX)
    {
        // custom chart에서 들어오는 index가 여기의 vector index와 다르기 때문에 여기서 새로 찾아야 함.
        for (int i = 0; i < this->qpoints.size(); i++)
        {
            if (qAbs(this->qpoints[i].x() - x) < SimpConstValue::MARGIN_CHART_CLICK && qAbs(this->qpoints[i].y() - y) < SimpConstValue::MARGIN_CHART_CLICK)
            {
                this->highlightPointIndex = i;
                DialogContrastCurve::UpdateSpinUI(this->qpoints[i].x(), this->qpoints[i].y(), true);
                return;
            }
        }

        // 선택된게 없었으면 add point
        DialogContrastCurve::AddPoint(x, y);
    }
}

void DialogContrastCurve::handlePointMoved(const QPointF &newPos)
{
    if (this->highlightPointIndex > -1)
    {
        QPointF pos = newPos;

        // cap을 씌운다.
        if (pos.x() < 0)
        {
            pos.setX(0);
        }
        else if (pos.x() > SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MAX)
        {
            pos.setX(SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MAX);
        }

        if (pos.y() < 0)
        {
            pos.setY(0);
        }
        else if (pos.y() > SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MAX)
        {
            pos.setY(SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MAX);
        }

        this->qpoints[this->highlightPointIndex] = pos;
        // 이동 중에는 sort를 하지 않는다.

        DialogContrastCurve::UpdateSpinUI(pos.x(), pos.y(), true);
        DialogContrastCurve::UpdateChart(this->qpoints, this->highlightPointIndex);
    }
}

void DialogContrastCurve::handlePointMovingFinished()
{
    // 이동이 끝나면 sort
    std::sort(this->qpoints.begin(), this->qpoints.end(), [](const QPointF &a, const QPointF &b) { return a.x() < b.x(); });
    DialogContrastCurve::UpdateChart(this->qpoints, this->highlightPointIndex);
}

void DialogContrastCurve::EnableUI(bool enable)
{
    ui->cbPresets->setEnabled(enable);
    ui->btnRemovePreset->setEnabled(enable);
    ui->btnSavePreset->setEnabled(enable);
    ui->btnResetPreset->setEnabled(enable);
    ui->gvChartCurve->setEnabled(enable);
}

void DialogContrastCurve::UpdatePresetUI(const std::vector<PresetContrastCurve>& presets, const int index)
{
    ui->cbPresets->clear();

    if (presets.size() > 0)
    {
        for (const PresetContrastCurve& preset : presets)
        {
            QString message = QString("index: %1, points: %2")
                                  .arg(preset.GetIndex()) // 'f' for floating point, 2 decimal places
                                  .arg(preset.GetPoints().size());
            ui->cbPresets->addItem(message);
        }
    }

    ui->cbPresets->setCurrentIndex(index);
}

void DialogContrastCurve::UpdateSpinUI(int x, int y, bool enable)
{
    {
        const QSignalBlocker blocker(ui->spinInput);
        ui->spinInput->setValue(x);
        ui->spinOutput->setValue(y);

        ui->spinInput->setEnabled(enable);
        ui->spinOutput->setEnabled(enable);
        ui->btnDeletePoint->setEnabled(enable);
    }
}

void DialogContrastCurve::InitChart()
{
    // Setup the chart
    this->chart->addSeries(this->splineSeries);
    this->chart->addSeries(this->scatterSeries);
    this->chart->addSeries(this->highlightSeries);
    this->chart->legend()->hide();

    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();

    // Setup the axes
    axisX->setRange(SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MIN, SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MAX);
    axisY->setRange(SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MIN, SimpConstValue::GEGL_CONTRAST_CURVE_VALUE_MAX);

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

void DialogContrastCurve::UpdateChart(const QVector<QPointF>& points, const int highlightIndex)
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

void DialogContrastCurve::AddPoint(const int x, const int y)
{
    // select point가 없었으면 point 추가
    DialogCurvePoint dialog(this->qpoints, x, y, this);

    if (dialog.exec() == QDialog::Accepted)
    {
        this->isPresetChanged = true;
        // 새로 추가한 후에 정렬한다.
        this->qpoints.emplaceBack(dialog.getInputValue(), dialog.getOutputValue());
        std::sort(this->qpoints.begin(), this->qpoints.end(), [](const QPointF &a, const QPointF &b) { return a.x() < b.x(); });
    }

    this->highlightPointIndex = -1;
    DialogContrastCurve::UpdateChart(this->qpoints, this->highlightPointIndex);
    DialogContrastCurve::UpdateSpinUI(0, 0, false);
}
