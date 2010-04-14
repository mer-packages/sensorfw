# temporary hack to allow direct linking to framework from client
# and avoid D-BUS for ariane purposes

TEMPLATE = lib
QT += dbus # TODO: remove
TARGET = directsensormanager

include( ../common-config.pri )

# TODO: start using plugins
HEADERS += ../sensord/filter.h \
    ../sensord/bin.h \
    ../sensord/consumer.h \
    ../sensord/ringbuffer.h \
    ../sensord/deviceadaptor.h \
    ../sensord/source.h \
    ../sensord/producer.h \
    ../sensord/sensormanager.h \
    ../sensord/sensormanager_a.h \
    ../sensord/plugin.h \
    ../sensord/loader.h \
    ../sensord/logging.h \
    ../sensord/abstractsensor_a.h \
    ../sensord/abstractsensor.h \
    ../sensord/parameterparser.h \
    ../examples/sensors/directsensormanager.h
SOURCES += ../sensord/filter.cpp \
    ../sensord/bin.cpp \
    ../sensord/consumer.cpp \
    ../sensord/ringbuffer.cpp \
    ../sensord/deviceadaptor.cpp \
    ../sensord/source.cpp \
    ../sensord/producer.cpp \
    ../sensord/sensormanager.cpp \
    ../sensord/sensormanager_a.cpp \
    ../sensord/plugin.cpp \
    ../sensord/loader.cpp \
    ../sensord/abstractsensor_a.cpp \
    ../sensord/abstractsensor.cpp \
    ../sensord/parameterparser.cpp \
    ../sensord/logging.cpp

SENSORFW_INCLUDEPATHS = ../.. \
    ../include

DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

include(../common-install.pri)
publicheaders.files = $$HEADERS
target.path = $$SHAREDLIBPATH
INSTALLS += target
