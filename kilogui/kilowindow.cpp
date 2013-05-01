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
    {"LedToggle", COMMAND_LEDTOGGLE},
    {"Bootload", BOOT},
    {"Reset", RESET},
    {"Sleep", SLEEP},
    {"Wakeup", WAKEUP},
    {"Charge", CHARGE},
    {"Voltage", VOLTAGE},
    {"Run", RUN},
};

static const int NUM_KILO_COMMANDS = sizeof(KILO_COMMANDS)/sizeof(kilo_cmd_t);


KiloWindow::KiloWindow(QWidget *parent): QWidget(parent) {
    QVBoxLayout *vbox = new QVBoxLayout;
    status = new QStatusBar();
    status->showMessage("Disconnected.");
    status->setSizeGripEnabled(false);
    connect_button = new QToolButton(status);
    status->addPermanentWidget(connect_button);
    connect_button->setText("Connect");
    connected = false;
    connect(connect_button, SIGNAL(clicked()), this, SLOT(toggleConnection()));

    QPushButton *serial_button = new QPushButton("Serial Input");
    serial = new TextWindow("Serial Input", this);
    QObject::connect(serial_button, SIGNAL(clicked()), this, SLOT(serialInput()));

    vbox->addWidget(createFileInput());
    vbox->addWidget(createCommands());
    vbox->addWidget(serial_button);
    vbox->addWidget(status);

    setLayout(vbox);
    setWindowTitle("Kilobots Toolkit");
    setWindowIcon(QIcon(":/images/kilogui.png"));

    conn = new FTDIConnection();
    QThread *thread = new QThread();

    connect(conn, SIGNAL(readText(QString)), serial, SLOT(addText(QString)));
    connect(conn, SIGNAL(error(QString)), this, SLOT(showError(QString)));
    connect(conn, SIGNAL(status(QString)), this, SLOT(showStatus(QString)));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    conn->moveToThread(thread);
    thread->start();
    conn->tryUSBOpen();
    conn->read();
}

void KiloWindow::serialInput() {
    serial->clear();
    serial->show();
}

void KiloWindow::showError(QString str) {
    QMessageBox::critical(this, "Kilobots Toolkit", str);
}

void KiloWindow::showStatus(QString str) {
    status->showMessage(str);
    if (str.startsWith("Connect")) {
        connected = true;
        connect_button->setText("Disconnect");
    } else {
        connected = false;
        connect_button->setText("Connect");
    }
}

void KiloWindow::toggleConnection() {
    if (connected)
        conn->close();
    else
        conn->tryUSBOpen();
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

    int split = NUM_KILO_COMMANDS/2;

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
    conn->sendCommand(packet);
}

void KiloWindow::program() {
    if (program_file.isEmpty()) {
         QMessageBox::critical(this, "Kilobots Toolkit", "You must select a program file to upload.");
    }
    else {
        conn->sendProgram(program_file);
    }
}
