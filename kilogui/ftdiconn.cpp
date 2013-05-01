#include "ftdiconn.h"
#include "packet.h"

static unsigned char buf[4096];
static uint8_t packet[PACKET_SIZE];

enum {
    MODE_NORMAL,
    MODE_UPLOAD,
};

FTDIConnection::FTDIConnection(QObject *parent): QObject(parent), ftdic(NULL), mode(MODE_NORMAL) { }

void FTDIConnection::read() {
    readLoop();
}

void FTDIConnection::close() {
    if (ftdic != NULL) {
        ftdi_usb_close(ftdic);
        ftdi_free(ftdic);
        ftdic = NULL;
        emit status("Disconnected.");
    }
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
            if (ftdi_set_baudrate(ftdic, 76800) != 0) {
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
    mode = MODE_NORMAL;
    if (ftdic != NULL) {
        if (ftdi_write_data(ftdic, (unsigned char *)cmd.constData(), cmd.length()) != cmd.length())
            emit error(QString(ftdic->error_str));
    } else {
        emit error("Cannot send command if disconnected from usb device.");
    }
}

void FTDIConnection::sendProgram(QString file) {
    if (ftdic != NULL) {
        data.load(file.toStdString());
        page_total = data.size()/PAGE_SIZE+1;
        if (page_total > 220)
            page_total = 220;
        page = page_total;
        if (mode != MODE_UPLOAD) {
            mode = MODE_UPLOAD;
            delay.start();
            QMetaObject::invokeMethod(this, "programLoop", Qt::QueuedConnection);
        }
    } else {
        emit error("Cannot upload program if disconnected from usb device.");
    }
}

void FTDIConnection::programLoop() {
    if (ftdic != NULL && delay.elapsed() > 130) {
        if (page >= page_total) {
            page = 0;
            memset(packet, 0, PACKET_SIZE);
            packet[0] = PACKET_HEADER;
            packet[1] = PACKET_FORWARDMSG;
            packet[2] = page_total;
            packet[11] = BOOTPGM_SIZE;
            packet[PACKET_SIZE-1] = PACKET_HEADER^PACKET_FORWARDMSG^page_total^BOOTPGM_SIZE;
            if (ftdi_write_data(ftdic, packet, PACKET_SIZE) != PACKET_SIZE)
                emit error(QString(ftdic->error_str));
        } else {
            packet[0] = PACKET_HEADER;
            packet[1] = PACKET_BOOTPAGE;
            packet[2] = page;
            uint8_t checksum = PACKET_HEADER^PACKET_BOOTPAGE^page;
            uint8_t data_byte;
            for (int i=0; i<PAGE_SIZE; i++) {
                data_byte = data.get(page*PAGE_SIZE+i);
                packet[i+3] = data_byte;
                checksum ^= data_byte;
            }
            packet[PACKET_SIZE-1] = checksum;
            if (ftdi_write_data(ftdic, packet, PACKET_SIZE) != PACKET_SIZE)
                emit error(QString(ftdic->error_str));
            else
                page++;
        }
        delay.start();
    }
    if (mode == MODE_UPLOAD) {
        QMetaObject::invokeMethod(this, "programLoop", Qt::QueuedConnection);
    }
}

void FTDIConnection::readLoop() {
    if (ftdic != NULL) {
        int num = ftdi_read_data(ftdic, buf, 4096);
        if (num > 0)
            emit readText(QByteArray((char *)buf, num));
    }

    QMetaObject::invokeMethod(this, "readLoop", Qt::QueuedConnection);
}
