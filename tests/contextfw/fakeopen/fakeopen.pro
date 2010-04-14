TEMPLATE = lib
TARGET = fakeopen
DEPENDPATH += .
INCLUDEPATH += .
QT -= gui

# Input
HEADERS += fakeopen.h
SOURCES += fakeopen.cpp

include(../../../common-install.pri)
target.path = $$SHAREDLIBPATH
INSTALLS += target
