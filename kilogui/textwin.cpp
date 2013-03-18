#include <QtGui>
#include <QTextEdit>
#include "textwin.h"

TextWindow::TextWindow(QString title, QWidget *parent): QWidget(parent) {
    text_edit = new QTextEdit();
    text_edit->setReadOnly(true);
    text_edit->setLineWrapMode(QTextEdit::NoWrap);

    QPushButton *close_button = new QPushButton("C&lose");
    QPushButton *clear_button = new QPushButton("&Clear");
    QObject::connect(close_button, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(clear_button, SIGNAL(clicked()), this, SLOT(clear()));

    QVBoxLayout *layout = new QVBoxLayout();
    QHBoxLayout *hbox = new QHBoxLayout();
    layout->addWidget(text_edit);
    hbox->addWidget(clear_button);
    hbox->addWidget(close_button);
    layout->addLayout(hbox);
    setLayout(layout);
    setWindowTitle(title);
    setWindowFlags(Qt::WindowStaysOnTopHint|Qt::Dialog);
}

void TextWindow::clear() {
    text_edit->setPlainText("");
}

void TextWindow::addText(QString text) {
    if (this->isVisible()) {
        QString curtext = text_edit->toPlainText();
        curtext += text;
        text_edit->setPlainText(curtext);
        text_edit->moveCursor(QTextCursor::End);
        text_edit->ensureCursorVisible();
    }
    // QVerticalScrollbar *sb = text_edit->verticalScrollBar();
    // sb->triggerAction(QScrollBar::SliderToMaximum);
}
