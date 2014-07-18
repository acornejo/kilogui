#include <QApplication>
#include "kilowindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    /* app.setStyle("windowsvista"); */

    KiloWindow window;
    window.raise();
    window.show();

    return app.exec();
}
