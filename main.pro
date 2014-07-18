TEMPLATE = app
TARGET = kilogui
LIBS += -lftdi -lusb

RESOURCES = kilogui.qrc
RC_FILE = kilogui.rc
mac:ICON = images/kilogui.icns

HEADERS += kilowindow.h calibrate.h serialwin.h ftdiconn.h vusbconn.h serialconn.h

SOURCES += kilowindow.cpp kilogui.cpp calibrate.cpp serialwin.cpp ftdiconn.cpp intelhex.cpp vusbconn.cpp serialconn.cpp

mystaticconfig {
    QMAKE_LIBS_QT =
    QMAKE_LIBS_QT_THREAD =
    LIBS += $(QTDIR)/lib/libqt.a -lz -framework Carbon
}
