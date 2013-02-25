TEMPLATE = app
TARGET = kilogui
LIBS += -lftdi -lusb

RESOURCES = kilogui.qrc
RC_FILE = kilogui.rc

HEADERS += kilowindow.h

SOURCES += kilowindow.cpp kilogui.cpp
