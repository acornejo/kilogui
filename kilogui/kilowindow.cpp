#include <QtGui>
#include <QFileDialog>
#include <QStatusBar>
#include <QToolButton>
#include <ftdi.h>
#include "kilowindow.h"
#include "textwin.h"
#include "packet.h"

typedef struct {
    const char *name;
    const unsigned char type;
} kilo_cmd_t;

#define COMMAND_STOP 250
#define COMMAND_LEDTOGGLE 251

static const kilo_cmd_t KILO_COMMANDS[] = {
    {"Stop", COMMAND_STOP},
    {"Bootload", BOOT},
    {"Reset", RESET},
    {"Sleep", SLEEP},
    {"LedToggle", COMMAND_LEDTOGGLE},
    {"Idle", WAKEUP},
    {"Run", RUN},
    {"Voltage", VOLTAGE}
};

static const int NUM_KILO_COMMANDS = sizeof(KILO_COMMANDS)/sizeof(kilo_cmd_t);


KiloWindow::KiloWindow(QWidget *parent): QWidget(parent) {
    device = 0;
    QVBoxLayout *vbox = new QVBoxLayout;
    status = new QStatusBar();
    status->showMessage("Disconnected.");
    status->setSizeGripEnabled(false);
    connect_button = new QToolButton(status);
    status->addPermanentWidget(connect_button);
    connect_button->setText("Connect");
    connect(connect_button, SIGNAL(clicked()), this, SLOT(toggleConnection()));

    QPushButton *serial_button = new QPushButton("Serial Input");
    serial = new TextWindow("Serial Input", this);
    QObject::connect(serial_button, SIGNAL(clicked()), this, SLOT(serialInput()));

    vbox->addWidget(createDeviceSelect());
    vbox->addWidget(createFileInput());
    vbox->addWidget(createCommands());
    vbox->addWidget(serial_button);
    vbox->addWidget(status);

    setLayout(vbox);
    setWindowTitle("Kilobots Toolkit");
    setWindowIcon(QIcon(":/images/kilogui.png"));

    printf("Doing enumeration!\n");
    QVector<QString> ports = SerialConnection::enumerate();
    printf("Found %d ports\n", ports.size());
    for (int i = 0; i < ports.size(); i++) { 
        printf("%s\n", ports[i].toStdString().c_str());
    }
    fflush(stdout);

    QThread *thread = new QThread();
    vusb_conn = new VUSBConnection();
    ftdi_conn = new FTDIConnection();
    serial_conn = new SerialConnection();
    connect(ftdi_conn, SIGNAL(readText(QString)), serial, SLOT(addText(QString)));

    connect(vusb_conn, SIGNAL(error(QString)), this, SLOT(showError(QString)));
    connect(vusb_conn, SIGNAL(status(QString)), this, SLOT(vusbUpdateStatus(QString)));
    connect(ftdi_conn, SIGNAL(error(QString)), this, SLOT(showError(QString)));
    connect(ftdi_conn, SIGNAL(status(QString)), this, SLOT(ftdiUpdateStatus(QString)));
    connect(serial_conn, SIGNAL(error(QString)), this, SLOT(showError(QString)));
    connect(serial_conn, SIGNAL(status(QString)), this, SLOT(serialUpdateStatus(QString)));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    vusb_conn->moveToThread(thread);
    ftdi_conn->moveToThread(thread);
    serial_conn->moveToThread(thread);
    thread->start();
    vusb_conn->open();
    ftdi_conn->open();
    serial_conn->open();
}

void KiloWindow::serialInput() {
    serial->clear();
    serial->show();
    ftdi_conn->read();
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
    status->showMessage(str);
    if (str.startsWith("Connect")) {
        connect_button->setText("Disconnect");
    } else {
        connect_button->setText("Connect");
    }
}

void KiloWindow::toggleConnection() {
    if (device == 0) {
        if (ftdi_status.startsWith("Connect"))
            ftdi_conn->close();
        else
            ftdi_conn->open();
    } else if (device == 1) {
        if (vusb_status.startsWith("Connect"))
            vusb_conn->close();
        else
            vusb_conn->open();
    } else {
        if (serial_status.startsWith("Connect"))
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
    QRadioButton *vusb_button = new QRadioButton("VUSB");
    QRadioButton *serial_button = new QRadioButton("Serial");

    QObject::connect(ftdi_button, SIGNAL(clicked()), this, SLOT(selectFTDI()));
    QObject::connect(vusb_button, SIGNAL(clicked()), this, SLOT(selectVUSB()));
    QObject::connect(serial_button, SIGNAL(clicked()), this, SLOT(selectSerial()));

    ftdi_button->setChecked(true);

    hbox->addWidget(ftdi_button);
    hbox->addWidget(vusb_button);
    hbox->addWidget(serial_button);
    box->setLayout(hbox);

    return box;
}

QGroupBox *KiloWindow::createFileInput() {
    QGroupBox *box = new QGroupBox("Upload Program");
    QGridLayout *layout = new QGridLayout;

    QPushButton *program_button = new QPushButton("[select file]");
    QLabel *program_label = new QLabel("Program:");
    program_label->setBuddy(program_button);
    QPushButton *upload_button = new QPushButton("Upload");

    QObject::connect(program_button, SIGNAL(clicked()), this, SLOT(chooseProgramFile()));
    QObject::connect(upload_button, SIGNAL(clicked()), this, SLOT(program()));

    layout->addWidget(program_label, 0, 0);
    layout->addWidget(program_button, 0, 1);
    layout->addWidget(upload_button, 1, 0, 1, 2);

    box->setLayout(layout);

    return box;
}

QGroupBox *KiloWindow::createCommands() {
    QGroupBox *box = new QGroupBox("Send Commands");
    QButtonGroup *group = new QButtonGroup;
    QGridLayout *layout = new QGridLayout;

    int split = 3;

    for (int i=0; i<NUM_KILO_COMMANDS; i++) {
        QPushButton *button = new QPushButton(KILO_COMMANDS[i].name);
        group->addButton(button);
        group->setId(button, i);
        if (i > split) {
            layout->addWidget(button, i-split-1, 1);
        } else {
            layout->addWidget(button, i, 0);
        }
    }
    QObject::connect(group, SIGNAL(buttonClicked(int)), this, SLOT(sendMessage(int)));

    box->setLayout(layout);

    return box;
}

void KiloWindow::chooseProgramFile() {
    QString filename = QFileDialog::getOpenFileName(this, "Open Program File", "", "Program Hex File (*.hex)"); //launches File Selector
    program_file = filename;
    if (filename.isEmpty()) {
        ((QPushButton *)sender())->setText("[select file]");
    } else {
        QFileInfo info(filename);
        if (info.isReadable())
            ((QPushButton *)sender())->setText(info.fileName());
        else {
            QMessageBox::critical(this, "Kilobots Toolkit", "Unable to open program file for reading.");
            ((QPushButton *)sender())->setText("[select file]");
            program_file = "";
        }
    }
}

void KiloWindow::sendMessage(int index) {
    QByteArray packet(PACKET_SIZE, 0);
    unsigned char type = KILO_COMMANDS[index].type;
    if (type == COMMAND_STOP) {
        packet[0] = PACKET_HEADER;
        packet[1] = PACKET_STOP;
        packet[PACKET_SIZE-1]=PACKET_HEADER^PACKET_STOP;
    } else if (type == COMMAND_LEDTOGGLE) {
        packet[0] = PACKET_HEADER;
        packet[1] = PACKET_LEDTOGGLE;
        packet[PACKET_SIZE-1]=PACKET_HEADER^PACKET_LEDTOGGLE;
    } else {
        packet[0] = PACKET_HEADER;
        packet[1] = PACKET_FORWARDMSG;
        packet[11] = type;
        packet[PACKET_SIZE-1]=PACKET_HEADER^PACKET_FORWARDMSG^type;
    }
    if (device == 0)
        ftdi_conn->sendCommand(packet);
    else if (device == 1)
        vusb_conn->sendCommand(packet);
    else
        serial_conn->sendCommand(packet);
}

void KiloWindow::program() {
    if (program_file.isEmpty()) {
         QMessageBox::critical(this, "Kilobots Toolkit", "You must select a program file to upload.");
    }
    else {
        if (device == 0)
            ftdi_conn->sendProgram(program_file);
        else if (device == 1)
            vusb_conn->sendProgram(program_file);
        else
            serial_conn->sendProgram(program_file);
    }
}
