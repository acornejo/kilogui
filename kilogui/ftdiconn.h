#ifndef __FTDICONN_H__
#define __FTDICONN_H__

#include <ftdi.h>
#include <QObject>
#include <QString>

class FTDIConnection: public QObject {
    Q_OBJECT

private:
    struct ftdi_context *ftdic;

public:
    FTDIConnection(QObject *p=0);

signals:
    void readText(QString);
    void status(QString);
    void error(QString);

public slots:
    void sendCommand(QByteArray);
    void tryUSBOpen();
    void start();
};

#endif//__FTDICONN_H__
