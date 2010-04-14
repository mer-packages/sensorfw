QT += testlib \
    dbus
TEMPLATE = app
TARGET = sensor-example

include( ../../common-config.pri )

# TODO: start using plugins
HEADERS += sensorapitest.h \
    ../../sensord/filter.h \
    ../../sensord/bin.h \
    ../../sensord/consumer.h \
    ../../sensord/ringbuffer.h \
    ../../sensord/deviceadaptor.h \
    ../../sensord/source.h \
    ../../sensord/producer.h \
    ../../sensord/sensormanager.h \
    ../../sensord/sensormanager_a.h \
    ../../sensord/plugin.h \
    ../../sensord/loader.h \
    ../../sensord/logging.h \
    ../client/testclass.h \
    ../../sensord/abstractsensor_a.h \
    ../../sensord/abstractsensor.h \
    ../../sensord/parameterparser.h \
    directsensormanager.h
SOURCES += sensorapitest.cpp \
    ../../sensord/filter.cpp \
    ../../sensord/bin.cpp \
    ../../sensord/consumer.cpp \
    ../../sensord/ringbuffer.cpp \
    ../../sensord/deviceadaptor.cpp \
    ../../sensord/source.cpp \
    ../../sensord/producer.cpp \
    ../../sensord/sensormanager.cpp \
    ../../sensord/sensormanager_a.cpp \
    ../../sensord/plugin.cpp \
    ../../sensord/loader.cpp \
    ../../sensord/abstractsensor_a.cpp \
    ../../sensord/abstractsensor.cpp \
    ../../sensord/logging.cpp \
    ../../sensord/parameterparser.cpp
SENSORFW_INCLUDEPATHS = ../.. \
    ../client \
    ../../include \
    ../../filters \
    ../../sensord \
    ../../datatypes \
    ../../sensors/accelerometersensor \
    ../../sensors/orientationsensor \
    ../../sensors/tapsensor \
    ../../sensors/alssensor \
    ../../sensors/proximitysensor \
    ../../filters/orientationinterpreter \
    ../../adaptors/sysfsadaptor \
    ../../adaptors/magnetometeradaptor \
    ../../adaptors/accelerometeradaptor \
    ../../adaptors/alsadaptor \
    ../../adaptors/tapadaptor \
    ../../adaptors/proximityadaptor
DEPENDPATH += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS \
    ../../adaptors
LIBS += -L../../datatypes \
    -lsensordatatypes \
    -L../../filters \
    -lfilters
internal { 
    DEFINES += USE_INTERNAL
    LIBS += -L../../sensors/accelerometersensor \
        -laccelerometersensor \
        -L../../sensors/orientationsensor \
        -lorientationsensor \
        -L../../sensors/tapsensor \
        -ltapsensor \
        -L../../sensors/alssensor \
        -lalssensor \
        -L../../sensors/proximitysensor \
        -lproximitysensor \
        -L../../sensors/compasssensor \
        -lcompasssensor
}

# LIBS += -L../../libs/libak8974 -lak8974
ariane { 
    DEFINES += USE_ARIANE
    INCLUDEPATH += ../../sensors/arianeplugin \
        ../../sensors/filteredplugin
    LIBS += -L../../adaptors/arianeadaptor \
        -larianeadaptor \
        -L../../sensors/arianeplugin \
        -larianesensor \
        -L../../sensors/filteredplugin \
        -lfilteredsensor \
        -lariane \
        -lusb \
        -lbluetooth
}

#include(../../common-install.pri)
target.path = /usr/sbin/sensord-examples
INSTALLS += target
