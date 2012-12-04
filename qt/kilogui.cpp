#include <QApplication>
#include "kilowindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    KiloWindow window;
    window.show();

    return app.exec();
}
