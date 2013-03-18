#ifndef TEXTWINDOW_H
#define TEXTWINDOW_H

#include <QWidget>
#include <QString>

class QTextEdit;

class TextWindow: public QWidget {
    Q_OBJECT

public:
    TextWindow(QString title, QWidget *parent = 0);

public slots:
    void addText(QString);
    void clear();

private:
    QTextEdit *text_edit;
};

#endif//TEXTWINDOW_H
