#ifndef KILOWINDOW_H
#define KILOWINDOW_H

#include <QWidget>
#include <QProcess>

class QGroupBox;

class KiloWindow: public QWidget {
    Q_OBJECT

public:
    KiloWindow(QWidget *parent = 0);

private:
    QString control_file;
    QString program_file;
    QString temp_file;
    QGroupBox *createFileInput();
    QGroupBox *createCommands();

private slots:
    void program();
    void programError(QProcess::ProcessError);
    void programFinished(int, QProcess::ExitStatus);
    void chooseControlFile();
    void chooseProgramFile();
    void sendCommand(int);
};

#endif//KILOWINDOW_H
