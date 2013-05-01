#ifndef KILOWINDOW_H
#define KILOWINDOW_H

#include <QWidget>
#include "textwin.h"
#include "ftdiconn.h"

class QGroupBox;
class QToolButton;
class QStatusBar;

class KiloWindow: public QWidget {
    Q_OBJECT

public:
    KiloWindow(QWidget *parent = 0);

private:
    bool connected;
    FTDIConnection *conn;
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
