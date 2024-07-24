#include "../include/customplaintextedit.h"
#include <QKeyEvent>

CustomPlainTextEdit::CustomPlainTextEdit(QWidget *parent)
    : QPlainTextEdit(parent) {}

void CustomPlainTextEdit::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        emit editingFinished();
    } else {
        QPlainTextEdit::keyPressEvent(event);
    }
}

void CustomPlainTextEdit::focusOutEvent(QFocusEvent *event) {
    QPlainTextEdit::focusOutEvent(event);
    emit editingFinished();
}
