QT += core dbus network
QT -= gui

include(../common-install.pri)

CONFIG += debug
TEMPLATE = app
TARGET = sensortestapp

TARGET_LIB = $$[TARGET_LIB]
isEmpty(TARGET_LIB) {
    TARGET_LIB = qtapi
}

HEADERS += parser.h \
           statprinter.h \
           abstractsensorhandler.h \
           clientadmin.h

SOURCES += main.cpp \
           parser.cpp \
           statprinter.cpp \
           abstractsensorhandler.cpp \
           clientadmin.cpp

SENSORFW_INCLUDEPATHS = ../../include \
                        ../../filters \
                        ../../datatypes \
                        ../../core \
			../..

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

QMAKE_LIBDIR_FLAGS += -L../../qt-api -lsensorclient \
                      -L../../datatypes -lsensordatatypes \
                      -L../../core -lsensorfw

DEFINES += TARGET_LIB_$$TARGET_LIB

message("Compiling testapp for $$TARGET_LIB")
equals(TARGET_LIB,qtapi) {
    HEADERS += sensorhandler_qtapi.h
    SOURCES += sensorhandler_qtapi.cpp
    INCLUDEPATH += ../../qt-api
}
equals(TARGET_LIB,qmsystem2) {
    HEADERS += sensorhandler_qmsystem2.h
    SOURCES += sensorhandler_qmsystem2.cpp
    INCLUDEPATH += /usr/include/qmsystem2
    LIBS += -lqmsystem2
}
equals(TARGET_LIB,qtmob) {
    HEADERS += sensorhandler_qtmob.h
    SOURCES += sensorhandler_qtmob.cpp
    MOBILITY += sensors
    CONFIG += mobility
    LIBS += -lQtSensors
}

testconf.path = /usr/share/sensorfw-tests/
testconf.files = testapp.conf

INSTALLS += testconf
