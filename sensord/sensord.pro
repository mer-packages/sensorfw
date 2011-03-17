QT += network

TEMPLATE = app
TARGET = sensord

include( ../common-config.pri )

CONFIG += console \
          link_pkgconfig

SENSORFW_INCLUDEPATHS = .. \
                        ../include \
                        ../filters \
                        ../datatypes \
                        ../core
DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

QMAKE_LIBDIR_FLAGS += -L../datatypes -lsensordatatypes \
                      -L../core -lsensorfw

QMAKE_RPATHDIR += /usr/lib/sensord

SOURCES += main.cpp \
           parser.cpp

HEADERS += parser.h

contextprovider {
    DEFINES += PROVIDE_CONTEXT_INFO
    PKGCONFIG += contextprovider-1.0
}

TARGET_H.path += /usr/include/sensord
TARGET_H.files = $$HEADERS
target.path = /usr/sbin/

INSTALLS += target \
            TARGET_H
