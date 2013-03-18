#include "ftdiconn.h"

static unsigned char buf[4096];

FTDIConnection::FTDIConnection(QObject *parent): QObject(parent), ftdic(NULL) {
}

void FTDIConnection::start() {
    if (ftdic != NULL) {
        int num = ftdi_read_data(ftdic, buf, 4096);
        if (num > 0)
            emit readText(QByteArray((char *)buf, num));
    }

    QMetaObject::invokeMethod(this, "start", Qt::QueuedConnection);
}

void FTDIConnection::tryUSBOpen() {
    QString status_msg = "Connected.";
    unsigned int chipid;

    if (ftdic != NULL) {
        if (ftdi_read_chipid(ftdic, &chipid) == 0)
            return;
        else {
            ftdi_usb_close(ftdic);
            ftdi_free(ftdic);
            ftdic = NULL;
        }
    }

    if (ftdic == NULL) {
        ftdic = ftdi_new();
        ftdic->usb_read_timeout = 5000;
        ftdic->usb_write_timeout = 1000;
        if (ftdi_usb_open(ftdic, 0x0403, 0x6001) != 0) {
            status_msg = QString("Disconnected: %1").arg(ftdic->error_str);
            ftdi_free(ftdic);
            ftdic = NULL;
        } else {
            if (ftdi_set_baudrate(ftdic, 19200) != 0) {
                status_msg = QString("Disconnected: %1").arg(ftdic->error_str);
                ftdi_usb_close(ftdic);
                ftdi_free(ftdic);
                ftdic = NULL;
            } else if (ftdi_setflowctrl(ftdic, SIO_DISABLE_FLOW_CTRL) != 0) {
                status_msg = QString("Disconnected: %1").arg(ftdic->error_str);
                ftdi_usb_close(ftdic);
                ftdi_free(ftdic);
                ftdic = NULL;
            } else if (ftdi_set_line_property(ftdic, BITS_8, STOP_BIT_1, NONE) != 0) {
                status_msg = QString("Disconnected: %1").arg(ftdic->error_str);
                ftdi_usb_close(ftdic);
                ftdi_free(ftdic);
                ftdic = NULL;
            }
        }
    }

    emit status(status_msg);
}

void FTDIConnection::sendCommand(QByteArray cmd) {
    if (ftdic != NULL) {
        if (ftdi_write_data(ftdic, (unsigned char *)cmd.constData(), cmd.length()) != cmd.length())
            emit error(QString(ftdic->error_str));
    } else {
        emit error("Cannot send command if disconnected from usb device.");
    }
}
