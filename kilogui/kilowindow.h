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
class QToolButton;
class QStatusBar;

class KiloWindow: public QWidget {
    Q_OBJECT

public:
    KiloWindow(QWidget *parent = 0);

private:
<<<<<<< HEAD
#ifdef DIGI
    DigiConnection *conn;
#else
=======
    bool connected;
>>>>>>> 8e6b6fa02dbf8d1bec7079a4649e09888e2188a5
    FTDIConnection *conn;
#endif
    QString program_file;
    
    QStatusBar *status;
    QToolButton *connect_button;
    TextWindow *serial;
    QGroupBox *createFileInput();
    QGroupBox *createCommands();

private slots:
    void toggleConnection();
    void showError(QString);
    void showStatus(QString);
    void program();
    void chooseProgramFile();
    void serialInput();
    void sendMessage(int);
};

#endif//KILOWINDOW_H
