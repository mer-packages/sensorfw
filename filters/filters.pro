TEMPLATE = lib
QT += dbus # TODO: remove
QT -= gui
TARGET = filters

include( ../common-config.pri )

DEPENDPATH += ..
INCLUDEPATH += ..
HEADERS += utils.h \
    timedunsigned.h \
    genericdata.h \
    orientationdata.h

SOURCES += utils.cpp

LIBS += -lrt

internal {
    HEADERS += posedata.h \
        tapdata.h \
        touchdata.h
 }

include(../common-install.pri)
publicheaders.path = $${publicheaders.path}/filters
publicheaders.files = $$HEADERS

target.path = $$SHAREDLIBPATH

INSTALLS += target
