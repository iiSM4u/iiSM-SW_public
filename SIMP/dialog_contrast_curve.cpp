#include "dialog_contrast_curve.h"
#include "ui_dialog_contrast_curve.h"

dialog_contrast_curve::dialog_contrast_curve(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialog_contrast_curve)
{
    ui->setupUi(this);

    // ui->plotCurve->addGraph();
    // ui->plotCurve->xAxis->setRange(0, 255);
    // ui->plotCurve->yAxis->setRange(0, 255);
    // ui->plotCurve->xAxis->setLabel("Input");
    // ui->plotCurve->yAxis->setLabel("Output");

    // // Create data points for the curve
    // QVector<double> x(5), y(5);
    // x[0] = 0;   y[0] = 0;
    // x[1] = 64;  y[1] = 80;
    // x[2] = 128; y[2] = 160;
    // x[3] = 192; y[3] = 220;
    // x[4] = 255; y[4] = 255;

    // ui->plotCurve->graph(0)->setData(x, y);

    // // Make the curve line smoother
    // ui->plotCurve->graph(0)->setPen(QPen(Qt::blue)); // Curve color
    // ui->plotCurve->graph(0)->setLineStyle(QCPGraph::lsLine);
    // ui->plotCurve->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));

    // // Enable user interactions
    // ui->plotCurve->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    // // Replot to update the view
    // ui->plotCurve->replot();

}

dialog_contrast_curve::dialog_contrast_curve(bool enable, std::vector<preset_contrast_curve>& presets, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::dialog_contrast_curve)
    , presets(presets)
{
    ui->setupUi(this);

    connect(ui->chkCurve, &QCheckBox::checkStateChanged, this, &dialog_contrast_curve::chkCurve_CheckedChanged);
    connect(ui->cbPresets, &QComboBox::currentIndexChanged, this, &dialog_contrast_curve::cbPreset_SelectedIndexChanged);
    connect(ui->btnSavePreset, &QPushButton::clicked, this, &dialog_contrast_curve::btnSavePreset_Click);

    // // combobox를 업데이트하면서 slider가 업데이트 되기 때문에 slider 보다 먼저 combobox를 업데이트한다.
    dialog_contrast_curve::UpdatePresetUI(this->presets);

    ui->chkCurve->setCheckState(enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    dialog_contrast_curve::EnableUI(enable);
}


dialog_contrast_curve::~dialog_contrast_curve()
{
    delete ui;
}

bool dialog_contrast_curve::getEnable() const
{
    return ui->chkCurve->isChecked();
}

int dialog_contrast_curve::getSelectedIndex() const
{
    return ui->cbPresets->currentIndex();
}

void dialog_contrast_curve::chkCurve_CheckedChanged(Qt::CheckState checkState)
{
    dialog_contrast_curve::EnableUI(checkState == Qt::CheckState::Checked);
}

void dialog_contrast_curve::cbPreset_SelectedIndexChanged(int index)
{
    if (index > -1)
    {
        preset_contrast_curve preset = this->presets[index];

    }
}

void dialog_contrast_curve::btnSavePreset_Click()
{
    bool ok;
    //double brightness = ui->editBrightness->toPlainText().toDouble(&ok);
    //double contrast = ui->editContrast->toPlainText().toDouble(&ok);
    int index = this->presets.size();

    //this->presets.emplace_back(index, brightness, contrast);

    dialog_contrast_curve::UpdatePresetUI(this->presets, index);
}

void dialog_contrast_curve::EnableUI(bool enable)
{
    ui->cbPresets->setEnabled(enable);
    ui->btnSavePreset->setEnabled(enable);
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
