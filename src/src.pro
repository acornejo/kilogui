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

unix {
  #VARIABLES
  isEmpty(PREFIX) {
    PREFIX = /usr
  }
  BINDIR = $$PREFIX/bin
  DATADIR =$$PREFIX/share

  DEFINES += DATADIR=\\\"$$DATADIR\\\" PKGDATADIR=\\\"$$PKGDATADIR\\\"

  #MAKE INSTALL

  INSTALLS += target desktop icon64

  target.path =$$BINDIR

  desktop.path = $$DATADIR/applications/hildon
  desktop.files += $${TARGET}.desktop

  icon64.path = $$DATADIR/icons/hicolor/64x64/apps
  icon64.files += ../data/64x64/$${TARGET}.png
}
