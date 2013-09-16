QT += network

TEMPLATE = lib
TARGET = hybrissensorfw

include( ../common-config.pri )

CONFIG += link_pkgconfig

SENSORFW_INCLUDEPATHS = .. \
                        ../include \
                        ../filters \
                        ../datatypes

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

QMAKE_LIBDIR_FLAGS += -lsensordatatypes-qt5

SOURCES += hybrisadaptor.cpp
HEADERS += hybrisadaptor.h
LIBS += -L/usr/lib -lhybris-common -lhardware

include(../common-install.pri)
target.path = $$SHAREDLIBPATH
INSTALLS += target
