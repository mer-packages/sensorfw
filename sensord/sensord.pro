QT += dbus network
QT -= gui

TEMPLATE = app
TARGET = sensord

include( ../common-config.pri )

CONFIG += console \
          link_pkgconfig
CONFIG -= app_bundle

PKGCONFIG += contextprovider-1.0


SENSORFW_INCLUDEPATHS = .. \
                        ../include \
                        ../filters \
                        ../datatypes
DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

LIBS += -Wl,-rpath,/usr/lib/sensord
LIBS += -Wl,-E

QMAKE_LIBDIR_FLAGS += -L../datatypes -lsensordatatypes

SOURCES += main.cpp \
    sensormanager.cpp \
    sensormanager_a.cpp \
    ringbuffer.cpp \
    producer.cpp \
    source.cpp \
    consumer.cpp \
    bin.cpp \
    filter.cpp \
    deviceadaptor.cpp \
    loader.cpp \
    plugin.cpp \
    filterproperty.cpp \
    abstractsensor_a.cpp \
    abstractsensor.cpp \
    logging.cpp \
    parameterparser.cpp \
    sysfsadaptor.cpp \
    sockethandler.cpp \
    inputdevadaptor.cpp \
    config.cpp \
    propertyhandler.cpp \
    calibrationhandler.cpp \
    nodebase.cpp \
    parser.cpp

HEADERS += sensormanager.h \
    sensormanager_a.h \
    dbusemitter.h \
    pusher.h \
    ringbuffer.h \
    producer.h \
    callback.h \
    source.h \
    consumer.h \
    sink.h \
    bin.h \
    filter.h \
    deviceadaptor.h \
    deviceadaptorringbuffer.h \
    bufferreader.h \
    loader.h \
    plugin.h \
    filterproperty.h \
    abstractsensor_a.h \
    abstractsensor.h \
    logging.h \
    parameterparser.h \
    abstractchain.h \
    sysfsadaptor.h \
    sockethandler.h \
    inputdevadaptor.h \
    config.h \
    propertyhandler.h \
    calibrationhandler.h \
    nodebase.h \
    parser.h

!mce_disable {
    SOURCES += mcewatcher.cpp
    HEADERS += mcewatcher.h
    DEFINES += SENSORFW_MCE_WATCHER
}

internal:DEFINES += USE_INTERNAL

use_syslog:DEFINES += SENSORD_USE_SYSLOG

contextprovider:DEFINES += PROVIDE_CONTEXT_INFO

TARGET_H.path += /usr/include/sensord
TARGET_H.files = $$HEADERS
target.path = /usr/sbin/

INSTALLS += target \
            TARGET_H
