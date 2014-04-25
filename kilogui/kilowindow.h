#ifndef KILOWINDOW_H
#define KILOWINDOW_H

#include <QWidget>
#include "textwin.h"
#include "vusbconn.h"
#include "ftdiconn.h"
#include "serialconn.h"

class QGroupBox;
class QToolButton;
class QStatusBar;

class KiloWindow: public QWidget {
    Q_OBJECT

public:
    KiloWindow(QWidget *parent = 0);

private:
    VUSBConnection *vusb_conn;
    FTDIConnection *ftdi_conn;
    SerialConnection *serial_conn;
    QString vusb_status;
    QString ftdi_status;
    QString serial_status;
    int device;
    QString program_file;

    QStatusBar *status;
    QToolButton *connect_button;
    TextWindow *serial;
    QGroupBox *createFileInput();
    QGroupBox *createCommands();
    QGroupBox *createDeviceSelect();
    void updateStatus();

private slots:
    void selectFTDI();
    void selectVUSB();
    void selectSerial();
    void toggleConnection();
    void showError(QString);
    void serialUpdateStatus(QString);
    void ftdiUpdateStatus(QString);
    void vusbUpdateStatus(QString);
    void program();
    void chooseProgramFile();
    void serialInput();
    void sendMessage(int);
};

#endif//KILOWINDOW_H
