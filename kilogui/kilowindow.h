#ifndef KILOWINDOW_H
#define KILOWINDOW_H

#include <QWidget>
#include <QProcess>
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
    QString control_file;
    QString program_file;
    QString temp_file;
    QStatusBar *status;
    TextWindow *serial;
    QGroupBox *createFileInput();
    QGroupBox *createCommands();

private slots:
    void showError(QString);
    void showStatus(QString);
    void program();
    void programError(QProcess::ProcessError);
    void programFinished(int, QProcess::ExitStatus);
    void chooseControlFile();
    void chooseProgramFile();
    void serialInput();
    void sendCommand(int);
};

#endif//KILOWINDOW_H
