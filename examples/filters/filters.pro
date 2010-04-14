QT += testlib \
    dbus
TEMPLATE = app
TARGET = filter-example

include( ../../common-config.pri )

HEADERS += filterapitest.h \
    ../../sensord/producer.h \
    ../../sensord/consumer.h \
    ../../sensord/bin.h \
    ../../sensord/ringbuffer.h \
    ../../sensord/filter.h \
    ../../sensord/source.h \
    ../../sensord/deviceadaptor.h \
    ../../sensord/plugin.h \
    ../../sensord/sensormanager.h \
    ../../sensord/loader.h \
    ../../sensord/logging.h \
    ../../sensord/sensormanager_a.h \
    ../../sensord/parameterparser.h
SOURCES += filterapitest.cpp \
    ../../sensord/producer.cpp \
    ../../sensord/consumer.cpp \
    ../../sensord/bin.cpp \
    ../../sensord/ringbuffer.cpp \
    ../../sensord/filter.cpp \
    ../../sensord/source.cpp \
    ../../sensord/deviceadaptor.cpp \
    ../../sensord/plugin.cpp \
    ../../sensord/sensormanager.cpp \
    ../../sensord/loader.cpp \
    ../../sensord/logging.cpp \
    ../../sensord/sensormanager_a.cpp \
    ../../sensord/parameterparser.cpp
INCLUDEPATH += ../.. \
    ../../include \
    ../../filters
LIBS += -L../../filters \
    -lfilters
ariane {
DEFINES += USE_ARIANE
LIBS += -L../../adaptors/arianeadaptor \
    -larianeadaptor \
    -lariane \
    -lusb \
    -lbluetooth
}
compass:# LIBS += -L../../sensors/compassplugin -lcompasssensor

# LIBS += -L../../libs/libak8974 -lak8974
# INCLUDEPATH += ../../libs
DEFINES += USE_LIBAK

#include(../../common-install.pri)
target.path = /usr/sbin/sensord-examples
INSTALLS += target
