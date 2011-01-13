QT += dbus network

include(../common-install.pri)

LIBS += -Wl,-rpath,/usr/lib/sensord,-E

TEMPLATE = app
TARGET = sensordataflow-test

CONFIG += testcase

HEADERS += dataflowtests.h \



SOURCES += dataflowtests.cpp \


INCLUDEPATH += ../.. \
    ../../include \
    ../../chains \
    ../../core \
    ../../datatypes \
    ../../chains \
    ../../filters \
    ../../adaptors \
    ../../adaptors/sysfsadaptor 

QMAKE_LIBDIR_FLAGS += -L../../builddir/datatypes -L../../datatypes/ -lsensordatatypes
QMAKE_LIBDIR_FLAGS += -L../../builddir/core -L../../core/ -lsensorfw
