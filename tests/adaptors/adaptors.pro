QT += testlib dbus network
QT -= gui

include(../common-install.pri)

CONFIG += debug
TEMPLATE = app
TARGET = sensoradaptors-test

HEADERS += adaptortest.h \
    ../../sensord/producer.h \
    ../../sensord/consumer.h \
    ../../sensord/bin.h \
    ../../sensord/ringbuffer.h \
    ../../sensord/filter.h \
    ../../sensord/source.h \
    ../../sensord/deviceadaptor.h \
    ../../sensord/sensormanager.h \
    ../../sensord/sensormanager_a.h \
    ../../sensord/loader.h \
    ../../sensord/plugin.h \
    ../../sensord/filterproperty.h \
    ../../sensord/abstractsensor.h \
    ../../sensord/logging.h \
    ../../sensord/config.h \
    ../../sensord/parameterparser.h \
    ../../sensord/sysfsadaptor.h \
    ../../sensord/inputdevadaptor.h \
    ../../filters/utils.h \
    ../../sensord/sockethandler.h \
    ../../sensord/propertyhandler.h \
    ../../sensord/calibrationhandler.h \
    ../../adaptors/accelerometeradaptor/accelerometeradaptor.h \
    ../../adaptors/magnetometeradaptor/magnetometeradaptor.h \
    ../../adaptors/alsadaptor/alsadaptor.h \
    ../../adaptors/tapadaptor/tapadaptor.h \
    ../../adaptors/touchadaptor/touchadaptor.h \
    ../../adaptors/kbslideradaptor/kbslideradaptor.h \
    ../../adaptors/proximityadaptor/proximityadaptor.h

SOURCES += adaptortest.cpp \
    ../../sensord/producer.cpp \
    ../../sensord/consumer.cpp \
    ../../sensord/bin.cpp \
    ../../sensord/ringbuffer.cpp \
    ../../sensord/filter.cpp \
    ../../sensord/source.cpp \
    ../../sensord/deviceadaptor.cpp \
    ../../sensord/sensormanager.cpp \
    ../../sensord/sensormanager_a.cpp \
    ../../sensord/loader.cpp \
    ../../sensord/plugin.cpp \
    ../../sensord/filterproperty.cpp \
    ../../sensord/abstractsensor.cpp \
    ../../sensord/logging.cpp \
    ../../sensord/config.cpp \
    ../../sensord/parameterparser.cpp \
    ../../sensord/sysfsadaptor.cpp \
    ../../sensord/inputdevadaptor.cpp \
    ../../filters/utils.cpp \
    ../../sensord/sockethandler.cpp \
    ../../sensord/propertyhandler.cpp \
    ../../sensord/calibrationhandler.cpp \
    ../../adaptors/accelerometeradaptor/accelerometeradaptor.cpp \
    ../../adaptors/magnetometeradaptor/magnetometeradaptor.cpp \
    ../../adaptors/alsadaptor/alsadaptor.cpp \
    ../../adaptors/tapadaptor/tapadaptor.cpp \
    ../../adaptors/touchadaptor/touchadaptor.cpp \
    ../../adaptors/kbslideradaptor/kbslideradaptor.cpp \
    ../../adaptors/proximityadaptor/proximityadaptor.cpp

INCLUDEPATH += ../.. \
    ../../include \
    ../../sensord \
    ../../datatypes \
    ../../filters \
    ../../adaptors/sysfsadaptor \
    ../../adaptors/touchadaptor \
    ../../adaptors/tapadaptor \
    ../../adaptors/alsadaptor \
    ../../adaptors/magnetometeradaptor \
    ../../adaptors/accelerometeradaptor \
    ../../adaptors/kbslideradaptor \
    ../../adaptors/proximityadaptor
    
