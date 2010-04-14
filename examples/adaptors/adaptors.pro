QT += testlib dbus
TEMPLATE = app
TARGET = adaptor-example

include( ../../common-config.pri )

HEADERS += adaptortest.h \
    ../../sensord/bin.h \
    ../../sensord/consumer.h \
    ../../sensord/ringbuffer.h \
    ../../sensord/deviceadaptor.h \
    ../../sensord/source.h \
    ../../sensord/producer.h \
    ../../filters/utils.h \
    ../../sensord/plugin.h \
    ../../sensord/loader.h \
    ../../sensord/sensormanager.h \
    ../../sensord/sensormanager_a.h \
    ../../sensord/logging.h \
    ../../sensord/parameterparser.h \
    ../../sensord/sysfsadaptor.h

SOURCES += adaptortest.cpp \
    ../../sensord/bin.cpp \
    ../../sensord/consumer.cpp \
    ../../sensord/ringbuffer.cpp \
    ../../sensord/deviceadaptor.cpp \
    ../../sensord/source.cpp \
    ../../sensord/producer.cpp \
    ../../filters/utils.cpp \
    ../../sensord/plugin.cpp \
    ../../sensord/loader.cpp \
    ../../sensord/sensormanager.cpp \
    ../../sensord/sensormanager_a.cpp \
    ../../sensord/logging.cpp \
    ../../sensord/parameterparser.cpp \
    ../../sensord/sysfsadaptor.cpp

INCLUDEPATH += ../.. \
    ../../include \
    ../../filters

internal {
    DEFINES += USE_INTERNAL
    LIBS += -L../../adaptors/magnetometeradaptor \
        -lmagnetometeradaptor \
        -L../../adaptors/accelerometeradaptor \
        -laccelerometeradaptor \
        -L../../adaptors/touchadaptor \
        -ltouchadaptor \
        -L../../adaptors/tapadaptor \
        -lalsadaptor \
        -L../../adaptors/alsadaptor \
        -ltapadaptor \
        -L../../adaptors/touchadaptor \
        -ltouchadaptor \
        -L../../adaptors/kbslideradaptor \
        -lkbslideradaptor \
        -L../../adaptors/proximityadaptor \
        -lproximityadaptor
INCLUDEPATH += ../../adaptors/touchadaptor \
        ../../adaptors/tapadaptor \
        ../../adaptors/alsadaptor \
        ../../adaptors/magnetometeradaptor \
        ../../adaptors/accelerometeradaptor \
        ../../adaptors/kbslideradaptor \
        ../../adaptors/proximityadaptor \
        ../../sensord
}

motionband {
    DEFINES += USE_MOTION_BAND
    INCLUDEPATH += ../../libs
    LIBS += -L../../libs/ht \
        -lmotionband
}

ariane {
    DEFINES += USE_ARIANE
    LIBS += -L../../adaptors/arianeadaptor \
        -larianeadaptor \
        -lariane \
        -lusb \
        -lbluetooth
}

#include(../../common-install.pri)
target.path = /usr/sbin/sensord-examples
INSTALLS += target
