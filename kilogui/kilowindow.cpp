#include <QtGui>
#include <QFileDialog>
#include <QStatusBar>
#include <QToolButton>
#include <ftdi.h>
#include "kilowindow.h"
#include "packet.h"

typedef struct {
    const char *name;
    const unsigned char type;
} kilo_cmd_t;

#define COMMAND_STOP 250
#define COMMAND_LEDTOGGLE 251

static calibmsg_t msg;

static const kilo_cmd_t KILO_COMMANDS[] = {
    {"Reset", RESET},
    {"Sleep", SLEEP},
    {"Run", RUN},
    {"Pause", WAKEUP},
    {"Voltage", VOLTAGE},
    {"LedToggle", COMMAND_LEDTOGGLE}
};

static const int NUM_KILO_COMMANDS = sizeof(KILO_COMMANDS)/sizeof(kilo_cmd_t);


KiloWindow::KiloWindow(QWidget *parent): QWidget(parent), device(0), sending(false), connected(false) {
    // Create status bar
    status = new QStatusBar();
    status->showMessage("disconnected.");
    status->setSizeGripEnabled(false);

    // Create status bar button
    connect_button = new QToolButton(status);
    status->addPermanentWidget(connect_button);
    connect_button->setText("Connect");
    connect(connect_button, SIGNAL(clicked()), this, SLOT(toggleConnection()));

    // Create serial input window and its trigger button
    serial_button = new QPushButton("Serial Input");
    serial = new SerialWindow("Serial Input", this);
    QObject::connect(serial_button, SIGNAL(clicked()), this, SLOT(serialShow()));

    // Create calibration window and its trigger button
    calib_button = new QPushButton("Calibration");
    calib = new CalibWindow("Calibration Values", this);
    QObject::connect(calib_button, SIGNAL(clicked()), this, SLOT(calibShow()));

    connect(calib, SIGNAL(calibUID(int)), this, SLOT(calibUID(int)));
    connect(calib, SIGNAL(calibLeft(int)), this, SLOT(calibLeft(int)));
    connect(calib, SIGNAL(calibRight(int)), this, SLOT(calibRight(int)));
    connect(calib, SIGNAL(calibStraight(int)), this, SLOT(calibStraight(int)));
    connect(calib, SIGNAL(calibStop()), this, SLOT(stopSending()));
    connect(calib, SIGNAL(calibSave()), this, SLOT(calibSave()));

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(createDeviceSelect());
    vbox->addWidget(createFileInput());
    vbox->addWidget(createCommands());
    vbox->addWidget(serial_button);
    vbox->addWidget(calib_button);
    vbox->addWidget(status);

    setLayout(vbox);
    setWindowTitle("Kilobots Toolkit");
    setWindowIcon(QIcon(":/images/kilogui.png"));
    setWindowState(Qt::WindowActive);

    printf("Doing enumeration!\n");
    QVector<QString> ports = SerialConnection::enumerate();
    printf("Found %d ports\n", ports.size());
    for (int i = 0; i < ports.size(); i++) { 
        printf("%s\n", ports[i].toStdString().c_str());
    }
    fflush(stdout);

    vusb_conn = new VUSBConnection();
    ftdi_conn = new FTDIConnection();
    serial_conn = new SerialConnection();
    connect(ftdi_conn, SIGNAL(readText(QString)), serial, SLOT(addText(QString)));
    connect(serial_conn, SIGNAL(readText(QString)), serial, SLOT(addText(QString)));

    connect(vusb_conn, SIGNAL(error(QString)), this, SLOT(showError(QString)));
    connect(vusb_conn, SIGNAL(status(QString)), this, SLOT(vusbUpdateStatus(QString)));
    connect(ftdi_conn, SIGNAL(error(QString)), this, SLOT(showError(QString)));
    connect(ftdi_conn, SIGNAL(status(QString)), this, SLOT(ftdiUpdateStatus(QString)));
    connect(serial_conn, SIGNAL(error(QString)), this, SLOT(showError(QString)));
    connect(serial_conn, SIGNAL(status(QString)), this, SLOT(serialUpdateStatus(QString)));

    // Create thread
    QThread *thread = new QThread();
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    // Move connections to thread
    vusb_conn->moveToThread(thread);
    ftdi_conn->moveToThread(thread);
    serial_conn->moveToThread(thread);

    // Start thread and open connections
    thread->start();
    vusb_conn->open();
    ftdi_conn->open();
    serial_conn->open();
}

void KiloWindow::serialShow() {
    serial->clear();
    serial->show();
    if (device == 0)
        ftdi_conn->read();
    if (device == 2)
        serial_conn->read();
}

void KiloWindow::calibShow() {
    calib->show();
}

void KiloWindow::showError(QString str) {
    QMessageBox::critical(this, "Kilobots Toolkit", str);
}

void KiloWindow::ftdiUpdateStatus(QString str) {
    ftdi_status = str;
    updateStatus();
}

void KiloWindow::vusbUpdateStatus(QString str) {
    vusb_status = str;
    updateStatus();
}

void KiloWindow::serialUpdateStatus(QString str) {
    serial_status = str;
    updateStatus();
}

void KiloWindow::updateStatus() {
    QString str = device == 0 ? ftdi_status : (device == 1 ? vusb_status : serial_status);
    QList<QPushButton*>::iterator i;
    status->showMessage(str);
    status->setToolTip(str);
    if (str.startsWith("connect")) {
        for (i = toggle_buttons.begin(); i != toggle_buttons.end(); i++) {
            (*i)->setEnabled(true);
        }
        serial_button->setEnabled(true);
        calib_button->setEnabled(true);
        if (program_file.isEmpty())
            upload_button->setEnabled(false);
        connected = true;
        connect_button->setText("Disconnect");
    } else {
        for (i = toggle_buttons.begin(); i != toggle_buttons.end(); i++) {
            (*i)->setEnabled(false);
        }
        connected = false;
        serial_button->setEnabled(false);
        calib_button->setEnabled(false);
        connect_button->setText("Connect");
    }
}

void KiloWindow::toggleConnection() {
    if (device == 0) {
        if (ftdi_status.startsWith("connect"))
            ftdi_conn->close();
        else
            ftdi_conn->open();
    } else if (device == 1) {
        if (vusb_status.startsWith("connect"))
            vusb_conn->close();
        else
            vusb_conn->open();
    } else {
        if (serial_status.startsWith("connect"))
            serial_conn->close();
        else
            serial_conn->open();
    }
}

void KiloWindow::selectFTDI() {
    device = 0;
    updateStatus();
}

void KiloWindow::selectVUSB() {
    device = 1;
    updateStatus();
}

void KiloWindow::selectSerial() {
    device = 2;
    updateStatus();
}

QGroupBox *KiloWindow::createDeviceSelect() {
    QGroupBox *box = new QGroupBox("Select Device");
    QHBoxLayout *hbox = new QHBoxLayout;
    QRadioButton *ftdi_button = new QRadioButton("FTDI");
    // QRadioButton *vusb_button = new QRadioButton("VUSB");
    QRadioButton *serial_button = new QRadioButton("Serial");

    QObject::connect(ftdi_button, SIGNAL(clicked()), this, SLOT(selectFTDI()));
    // QObject::connect(vusb_button, SIGNAL(clicked()), this, SLOT(selectVUSB()));
    QObject::connect(serial_button, SIGNAL(clicked()), this, SLOT(selectSerial()));

    ftdi_button->setChecked(true);

    hbox->addWidget(ftdi_button);
    // hbox->addWidget(vusb_button);
    hbox->addWidget(serial_button);
    box->setLayout(hbox);

    return box;
}

QGroupBox *KiloWindow::createFileInput() {
    QSignalMapper *mapper = new QSignalMapper(this);
    QGroupBox *box = new QGroupBox("Upload Program");
    QGridLayout *layout = new QGridLayout;

    QPushButton *choose_button = new QPushButton("[select file]");
    QLabel *program_label = new QLabel("Program:");
    program_label->setBuddy(choose_button);
    QPushButton *bootload_button = new QPushButton("Bootload");
    upload_button = new QPushButton("Upload");

    bootload_button->setCheckable(true);
    upload_button->setCheckable(true);
    upload_button->setEnabled(false);

    mapper->setMapping(bootload_button, BOOT);
    QObject::connect(bootload_button, SIGNAL(clicked()), mapper, SLOT(map()));
    QObject::connect(mapper, SIGNAL(mapped(int)), this, SLOT(sendMessage(int)));
    QObject::connect(choose_button, SIGNAL(clicked()), this, SLOT(chooseProgramFile()));
    QObject::connect(upload_button, SIGNAL(clicked()), this, SLOT(uploadProgram()));

    layout->addWidget(program_label, 0, 0);
    layout->addWidget(choose_button, 0, 1);
    layout->addWidget(bootload_button, 1, 0, 1, 2);
    layout->addWidget(upload_button, 2, 0, 1, 2);

    toggle_buttons.push_back(upload_button);
    toggle_buttons.push_back(bootload_button);

    box->setLayout(layout);

    return box;
}

QGroupBox *KiloWindow::createCommands() {
    QGroupBox *box = new QGroupBox("Send Commands");
    QSignalMapper *mapper = new QSignalMapper(this);
    QGridLayout *layout = new QGridLayout;

    int split = 2;

    for (int i=0; i<NUM_KILO_COMMANDS; i++) {
        QPushButton *button = new QPushButton(KILO_COMMANDS[i].name);
        if (KILO_COMMANDS[i].type != COMMAND_LEDTOGGLE) {
            button->setCheckable(true);
        }
        toggle_buttons.push_back(button);
        if (i > split) {
            layout->addWidget(button, i-split-1, 1);
        } else {
            layout->addWidget(button, i, 0);
        }
        mapper->setMapping(button, (int)KILO_COMMANDS[i].type);
        QObject::connect(button, SIGNAL(clicked()), mapper, SLOT(map()));
    }
    QObject::connect(mapper, SIGNAL(mapped(int)), this, SLOT(sendMessage(int)));

    box->setLayout(layout);

    return box;
}

void KiloWindow::chooseProgramFile() {
    QString filename = QFileDialog::getOpenFileName(this, "Open Program File", "", "Program Hex File (*.hex)"); //launches File Selector
    program_file = filename;
    upload_button->setEnabled(false);
    if (filename.isEmpty()) {
        ((QPushButton *)sender())->setText("[select file]");
    } else {
        QFileInfo info(filename);
        if (info.isReadable()) {
            ((QPushButton *)sender())->setText(info.fileName());
            if (connected)
                upload_button->setEnabled(true);
        }
        else {
            QMessageBox::critical(this, "Kilobots Toolkit", "Unable to open program file for reading.");
            ((QPushButton *)sender())->setText("[select file]");
            program_file = "";
        }
    }
}

void KiloWindow::uploadProgram() {
    if (sending) {
        stopSending();
    }
    if (program_file.isEmpty()) {
         QMessageBox::critical(this, "Kilobots Toolkit", "You must select a program file to upload.");
    }
    else {
        sending = true;
        if (device == 0)
            ftdi_conn->sendProgram(program_file);
        else if (device == 1)
            vusb_conn->sendProgram(program_file);
        else
            serial_conn->sendProgram(program_file);
    }
}

void KiloWindow::stopSending() {
    if (sending)
        sendMessage(COMMAND_STOP);
    QList<QPushButton*>::iterator i;
    for (i = toggle_buttons.begin(); i != toggle_buttons.end(); i++) {
        if ((*i)->isChecked())
            (*i)->setChecked(false);
    }
}

void KiloWindow::sendMessage(int type_int) {
    unsigned char type = (unsigned char)type_int;
    QByteArray packet(PACKET_SIZE, 0);
    if (type == COMMAND_STOP) {
        sending = false;
        packet[0] = PACKET_HEADER;
        packet[1] = PACKET_STOP;
        packet[PACKET_SIZE-1]=PACKET_HEADER^PACKET_STOP;
    } else  {
        if (sending) {
            stopSending();
            return;
        }

        if (type == COMMAND_LEDTOGGLE) {
            sending = false;
            packet[0] = PACKET_HEADER;
            packet[1] = PACKET_LEDTOGGLE;
            packet[PACKET_SIZE-1]=PACKET_HEADER^PACKET_LEDTOGGLE;
        } else {
            sending = true;
            packet[0] = PACKET_HEADER;
            packet[1] = PACKET_FORWARDMSG;
            packet[11] = type;
            packet[PACKET_SIZE-1]=PACKET_HEADER^PACKET_FORWARDMSG^type;
        }
    }

    if (device == 0)
        ftdi_conn->sendCommand(packet);
    else if (device == 1)
        vusb_conn->sendCommand(packet);
    else
        serial_conn->sendCommand(packet);
}

void KiloWindow::sendDataMessage(uint8_t *payload, uint8_t type) {
    if (sending)
        stopSending();

    QByteArray packet(PACKET_SIZE, 0);
    uint8_t checksum = PACKET_HEADER^PACKET_FORWARDMSG^type;
    packet[0] = PACKET_HEADER;
    packet[1] = PACKET_FORWARDMSG;
    for (int i = 0; i < 9; i++) {
        packet[2+i] = payload[i];
        checksum ^= payload[i];
    }
    packet[11] = type;
    packet[PACKET_SIZE-1] = checksum;
    sending = true;

    if (device == 0)
        ftdi_conn->sendCommand(packet);
    else if (device == 1)
        vusb_conn->sendCommand(packet);
    else
        serial_conn->sendCommand(packet);
}

void KiloWindow::calibSave() {
    msg.mode = CALIB_SAVE;
    sendDataMessage((uint8_t*)&msg, CALIB);
}
void KiloWindow::calibUID(int x) { 
    msg.mode = CALIB_UID;
    msg.uid = x;
    sendDataMessage((uint8_t *)&msg, CALIB);
}
void KiloWindow::calibLeft(int x) {
    msg.mode = CALIB_TURN_LEFT;
    msg.turn_left = x;
    sendDataMessage((uint8_t *)&msg, CALIB);
}
void KiloWindow::calibRight(int x) {
    msg.mode = CALIB_TURN_RIGHT;
    msg.turn_right = x;
    sendDataMessage((uint8_t *)&msg, CALIB);
}
void KiloWindow::calibStraight(int x) {
    msg.mode = CALIB_STRAIGHT;
    msg.straight_left = x&0xFF;
    msg.straight_right = (x>>8) & 0xFF;
    sendDataMessage((uint8_t *)&msg, CALIB);
}
