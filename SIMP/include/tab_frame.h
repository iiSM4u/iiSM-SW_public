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

    void lvFrame_Click(const QModelIndex& index);
    void btnOpenDir_Click();
    void btnZoomIn_Click();
    void btnZoomOut_Click();
    void btnImageProcessing_Click();

private:
    Ui::TabFrame *ui;

    QProgressDialog *progressDialog;
    QFileSystemModel *filesystemFrame;
    QString captureDir; // 생성자에서 초기화 함

    float zoomFactor = 1.0f;

    void ConnectUI();
    void InitUI();
};
