#ifndef KILOWINDOW_H
#define KILOWINDOW_H

#include <QWidget>
#include <QList>
#include <stdint.h>
#include "serialwin.h"
#include "vusbconn.h"
#include "ftdiconn.h"
#include "serialconn.h"
#include "calibrate.h"

class QGroupBox;
class QToolButton;
class QStatusBar;
class QPushButton;
class QTimer;

class KiloWindow: public QWidget {
    Q_OBJECT

public:
    KiloWindow(QWidget *parent = 0);

private:
    int device;
    VUSBConnection *vusb_conn;
    FTDIConnection *ftdi_conn;
    SerialConnection *serial_conn;
    QString vusb_status;
    QString ftdi_status;
    QString serial_status;
    QString program_file;

    QStatusBar *status;
    QToolButton *connect_button;
    SerialWindow *serial;
    CalibWindow *calib;
    QGroupBox *createFileInput();
    QGroupBox *createCommands();
    QGroupBox *createDeviceSelect();
    bool sending;
    bool connected;
    QPushButton *upload_button, *serial_button, *calib_button;
    QList<QPushButton*> toggle_buttons;
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
    void chooseProgramFile();
    void uploadProgram();
    void stopSending();
    void sendMessage(int);
    void sendDataMessage(uint8_t *, uint8_t);
    void serialShow();
    void calibShow();

    void calibUID(int);
    void calibLeft(int);
    void calibRight(int);
    void calibStraight(int);
    void calibSave();
};

#endif//KILOWINDOW_H
