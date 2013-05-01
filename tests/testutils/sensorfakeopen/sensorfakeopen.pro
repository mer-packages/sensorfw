TEMPLATE = lib
include(../../../common-install.pri)
TARGET = sensorfakeopen
DEPENDPATH += .
INCLUDEPATH += .
QT -= gui

# Input
HEADERS += sensorfakeopen.h
SOURCES += sensorfakeopen.cpp

target.path = $$SHAREDLIBPATH
INSTALLS += target
