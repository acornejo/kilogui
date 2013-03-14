#include <QtGui>
#include <QTextEdit>
#include <QScrollbar>
#include "textwin.h"

TextWindow::TextWindow(QString title, QWidget *parent): QWidget(parent) {
    text_edit = new QTextEdit();
    text_edit->setReadOnly(true);
    text_edit->setLineWrapMode(QTextEdit::NoWrap);

    QPushButton *close_button = new QPushButton("&Close");
    QObject::connect(close_button, SIGNAL(clicked()), this, SLOT(close()));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(text_edit);
    layout->addWidget(close_button);
    setLayout(layout);
    setWindowTitle(title);
    setWindowFlags(Qt::WindowStaysOnTopHint|Qt::Dialog);
}

void TextWindow::clear() {
    text_edit->setPlainText("");
}

void TextWindow::addText(QString text) {
    QString curtext = text_edit->toPlainText();
    curtext += text;
    text_edit->setPlainText(curtext);
    text_edit->moveCursor(QTextCursor::End);
    text_edit->ensureCursorVisible();
    // QVerticalScrollbar *sb = text_edit->verticalScrollBar();
    // sb->triggerAction(QScrollBar::SliderToMaximum);
}
