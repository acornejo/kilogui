#ifndef KILOWINDOW_H
#define KILOWINDOW_H

#include <QWidget>
#include "textwin.h"
#define DIGI
#ifdef DIGI
#include "digiconn.h"
#else
#include "ftdiconn.h"
#endif

class QGroupBox;
class QStatusBar;

class KiloWindow: public QWidget {
    Q_OBJECT

public:
    KiloWindow(QWidget *parent = 0);

private:
#ifdef DIGI
    DigiConnection *conn;
#else
    FTDIConnection *conn;
#endif
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
