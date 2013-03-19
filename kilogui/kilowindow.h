#ifndef KILOWINDOW_H
#define KILOWINDOW_H

#include <QWidget>
#include "textwin.h"
#include "ftdiconn.h"

class QGroupBox;
class QStatusBar;

class KiloWindow: public QWidget {
    Q_OBJECT

public:
    KiloWindow(QWidget *parent = 0);

private:
    FTDIConnection *conn;
    QString program_file;
    QStatusBar *status;
    TextWindow *serial;
    QGroupBox *createFileInput();
    QGroupBox *createCommands();

private slots:
    void showError(QString);
    void showStatus(QString);
    void program();
    void chooseProgramFile();
    void serialInput();
    void sendMessage(int);
};

#endif//KILOWINDOW_H
