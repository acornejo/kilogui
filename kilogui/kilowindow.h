#ifndef KILOWINDOW_H
#define KILOWINDOW_H

#include <QWidget>
#include <QProcess>
#include "textwin.h"
#include <ftdi.h>

class QGroupBox;
class QStatusBar;

class KiloWindow: public QWidget {
    Q_OBJECT

public:
    KiloWindow(QWidget *parent = 0);

private:
    struct ftdi_context *ftdic;
    QString control_file;
    QString program_file;
    QString temp_file;
    QStatusBar *status;
    TextWindow *serial;
    QGroupBox *createFileInput();
    QGroupBox *createCommands();

private slots:
    void tryOpenUSB();
    void program();
    void programError(QProcess::ProcessError);
    void programFinished(int, QProcess::ExitStatus);
    void chooseControlFile();
    void chooseProgramFile();
    void serialInput();
    void updateSerial();
    void sendCommand(int);
};

#endif//KILOWINDOW_H
