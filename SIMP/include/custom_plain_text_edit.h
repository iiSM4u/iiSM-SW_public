#pragma once

#include <QPlainTextEdit>

class CustomPlainTextEdit : public QPlainTextEdit {
    Q_OBJECT

public:
    explicit CustomPlainTextEdit(QWidget *parent = nullptr);

signals:
    void editingFinished();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
};

