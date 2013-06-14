#ifndef __DIGICONN_H__
#define __DIGICONN_H__

#include <usb.h>
#include <QObject>
#include <QString>
#include <QTime>
#include "intelhex.h"

class DigiConnection: public QObject {
    Q_OBJECT

private:
    struct usb_dev_handle *handle;
    intelhex::hex_data data;
    QTime delay;
    int mode;
    int page;
    int page_total;

public:
    DigiConnection(QObject *p=0);

signals:
    void status(QString);
    void error(QString);

public slots:
    void sendCommand(QByteArray);
    void sendProgram(QString);
    void tryUSBOpen();

private slots:
    void programLoop();
};

#endif//__DIGICONN_H__
