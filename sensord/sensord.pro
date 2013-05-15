QT += network

TEMPLATE = app

include( ../common-config.pri )
#don't change target name in qt5
TARGET = sensord

CONFIG += console \
          link_pkgconfig

SENSORFW_INCLUDEPATHS = .. \
                        ../include \
                        ../filters \
                        ../datatypes \
                        ../core \
                        ../sensors/magnetometersensor

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

QMAKE_LIBDIR_FLAGS += -L../datatypes \
                      -L../core

equals(QT_MAJOR_VERSION, 4):{
    TARGET_H.path += /usr/include/sensord
}
equals(QT_MAJOR_VERSION, 5):{
    TARGET_H.path += /usr/include/sensord-qt5
}
QMAKE_RPATHDIR += /usr/lib/sensord

SOURCES += main.cpp \
           parser.cpp \
           calibrationhandler.cpp

HEADERS += parser.h \
           calibrationhandler.h

contextprovider {
    DEFINES += PROVIDE_CONTEXT_INFO
    PKGCONFIG += contextprovider-1.0
}

TARGET_H.files = $$HEADERS
target.path = /usr/sbin/

INSTALLS += target \
            TARGET_H

include(../common.pri)

