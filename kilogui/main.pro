TEMPLATE = app
TARGET = kilogui
LIBS += -lftdi -lusb

RESOURCES = kilogui.qrc
RC_FILE = kilogui.rc
mac:ICON = images/kilogui.icns

HEADERS += kilowindow.h textwin.h ftdiconn.h

SOURCES += kilowindow.cpp kilogui.cpp textwin.cpp ftdiconn.cpp intelhex.cpp
