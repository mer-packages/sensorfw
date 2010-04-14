TEMPLATE = lib
TARGET = sensorfakeopen
DEPENDPATH += .
INCLUDEPATH += .
QT -= gui

# Input
HEADERS += sensorfakeopen.h
SOURCES += sensorfakeopen.cpp

include(../../../common-install.pri)
target.path = $$SHAREDLIBPATH
INSTALLS += target
