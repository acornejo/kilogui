#include <QtGui>
#include <QFileDialog>
#include <QStatusBar>
#include <ftdi.h>
#include "kilowindow.h"
#include "textwin.h"

typedef struct {
    const char *name;
    const char *cmd;
} kilo_cmd_t;

static const kilo_cmd_t KILO_COMMANDS[] = {{"Bootload", "a"}, {"Sleep", "b"}, {"Wakeup", "c"}, {"Pause", "d"}, {"Voltage", "e"}, {"Run", "f"}, {"Charge", "g"}, {"Stop", "h"}, {"LedsToggle", "i"}, {"BootloadMsg", "j"}, {"Reset", "z"}};

static const int NUM_KILO_COMMANDS = sizeof(KILO_COMMANDS)/sizeof(kilo_cmd_t);


KiloWindow::KiloWindow(QWidget *parent): QWidget(parent) {
    QVBoxLayout *vbox = new QVBoxLayout;
    status = new QStatusBar();
    status->showMessage("Disconnected.");
    status->setSizeGripEnabled(false);

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
    conn->start();

    QTimer *usbtimer = new QTimer();
    QObject::connect(usbtimer, SIGNAL(timeout()), conn, SLOT(tryUSBOpen()));
    usbtimer->start(1000*15); // try every 15 seconds

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
}


QGroupBox *KiloWindow::createFileInput() {
    QGroupBox *box = new QGroupBox("Program OHC");
    QGridLayout *layout = new QGridLayout;

    QPushButton *control_button = new QPushButton("[select file]");
    QPushButton *program_button = new QPushButton("[select file]");
    QLabel *control_label = new QLabel("Control:");
    control_label->setBuddy(control_button);
    QLabel *program_label = new QLabel("Program:");
    program_label->setBuddy(program_button);
    QPushButton *upload_button = new QPushButton("Upload");

    QObject::connect(control_button, SIGNAL(clicked()), this, SLOT(chooseControlFile()));
    QObject::connect(program_button, SIGNAL(clicked()), this, SLOT(chooseProgramFile()));
    QObject::connect(upload_button, SIGNAL(clicked()), this, SLOT(program()));

    layout->addWidget(control_label, 0, 0);
    layout->addWidget(control_button, 0, 1);
    layout->addWidget(program_label, 1, 0);
    layout->addWidget(program_button, 1, 1);
    layout->addWidget(upload_button, 2, 0, 1, 2);

    box->setLayout(layout);

    return box;
}

QGroupBox *KiloWindow::createCommands() {
    QGroupBox *box = new QGroupBox("Send Commands");
    QButtonGroup *group = new QButtonGroup;
    QGridLayout *layout = new QGridLayout;

    for (int i=0; i<NUM_KILO_COMMANDS; i++) {
        QPushButton *button = new QPushButton(KILO_COMMANDS[i].name);
        group->addButton(button);
        group->setId(button, i);
        if (i > NUM_KILO_COMMANDS/2) {
            layout->addWidget(button, i-NUM_KILO_COMMANDS/2-1, 1);
        } else {
            layout->addWidget(button, i, 0);
        }
    }
    QObject::connect(group, SIGNAL(buttonClicked(int)), this, SLOT(sendCommand(int)));

    box->setLayout(layout);

    return box;
}

void KiloWindow::chooseControlFile() {
    QString filename = QFileDialog::getOpenFileName(this, "Open Control File", "", "Control Hex File (*.hex)"); //launches File Selector
    control_file = filename;
    if (filename.isEmpty()) {
        ((QPushButton *)sender())->setText("[select file]");
    } else {
        QFileInfo info(filename);
        ((QPushButton *)sender())->setText(info.fileName());
    }
}

void KiloWindow::chooseProgramFile() {
    QString filename = QFileDialog::getOpenFileName(this, "Open Program File", "", "Program Hex File (*.hex)"); //launches File Selector
    program_file = filename;
    if (filename.isEmpty()) {
        ((QPushButton *)sender())->setText("[select file]");
    } else {
        QFileInfo info(filename);
        ((QPushButton *)sender())->setText(info.fileName());
    }
}

void KiloWindow::sendCommand(int index) {
    const char *cmd = KILO_COMMANDS[index].cmd;
    conn->sendCommand(QByteArray(cmd, strlen(cmd)));
}

void KiloWindow::program() {
    if (control_file.isEmpty() || program_file.isEmpty()) {
         QMessageBox::critical(
              this,
              "Kilobots Toolkit",
              "To program the kilobots you must select a control and program file first." );
    }
    else {
        QFile control(control_file);
        QFile program(program_file);
        QTemporaryFile output("merged-XXXXXX.hex");

        if (!control.open(QFile::ReadOnly)) {
             QMessageBox::critical(this, "Kilobots Toolkit", "Unable to open control file for reading.");
        } else if (!program.open(QFile::ReadOnly)) {
             QMessageBox::critical(this, "Kilobots Toolkit", "Unable to open program file for reading.");
             control.close();
        } else if (!output.open()) {
             QMessageBox::critical(this, "Kilobots Toolkit", "Unable to open temporary file for writing.");
             control.close();
             program.close();
        } else {
            // copy program except for last line
            char line[1024], oldline[1024]={'\0'};
            while (program.readLine(line, 1024) > 0) {
                if (strchr(line, ':') == NULL) break;
                output.write(oldline);
                strcpy(oldline,line);
            }

            // copy control (byte per byte)
            char ch;
            while (control.getChar(&ch)) {
                output.putChar(ch);
            }

            temp_file = output.fileName();
            output.setAutoRemove(false);

            control.close();
            program.close();
            output.close();

            QProcess *process = new QProcess(this);
            QString avrdude = "avrdude";
            QString flash_name = "flash:w:"+output.fileName();
            QStringList arguments;
            arguments << "-P" << "usb" << "-c" << "avrispmkII" << "-p" << "m328" << "-U" << flash_name;

            QObject::connect(process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(programError(QProcess::ProcessError)));
            QObject::connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(programFinished(int,QProcess::ExitStatus)));
            process->start(avrdude, arguments);
        }
    }
}

void KiloWindow::programFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    if (exitStatus == QProcess::CrashExit) {
        QMessageBox::critical(this, "Kilobots Toolkit", QString("Crash occurred when executing avrdude, exit code: %d.").arg(exitCode));
    } else {
        QMessageBox::information(this, "Kilobots Toolkit", "AvrDude executed successfully.");
    }
    QFile file(temp_file);
    file.remove();
}

void KiloWindow::programError(QProcess::ProcessError error) {
    QString errormessage;
    switch(error) {
        case QProcess::FailedToStart:
            errormessage = "Failed to start avrdude.";
            break;
        case QProcess::Crashed:
            errormessage = "Crash occurred executing avrdude.";
            break;
        case QProcess::Timedout:
            errormessage = "Time out executing avrdude.";
            break;
        case QProcess::WriteError:
            errormessage = "Write error executing avrdude.";
            break;
        case QProcess::ReadError:
            errormessage = "Read error executing avrdude.";
            break;
        case QProcess::UnknownError:
            errormessage = "Uknown error executing avrdude.";
            break;
    }
    QMessageBox::critical(this, "Kilobots Toolkit", errormessage);
    QFile file(temp_file);
    file.remove();
}
