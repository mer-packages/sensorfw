QT += dbus network

include(../common-install.pri)

LIBS += -Wl,-rpath,/usr/lib/sensord,-E

TEMPLATE = app
TARGET = sensordataflow-test

CONFIG += testcase

HEADERS += dataflowtests.h \
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
    ../../sensord/parameterparser.h \
    ../../sensord/logging.h \
    ../../sensord/config.h \
    ../../sensord/abstractchain.h \
    ../../sensord/abstractsensor.h \
    ../../sensord/sysfsadaptor.h \
    ../../sensord/inputdevadaptor.h \
    ../../sensord/sockethandler.h \
    ../../sensord/propertyhandler.h \
    ../../sensord/calibrationhandler.h

SOURCES += dataflowtests.cpp \
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
    ../../sensord/parameterparser.cpp \
    ../../sensord/logging.cpp \
    ../../sensord/config.cpp \
    ../../sensord/abstractsensor.cpp \
    ../../sensord/sysfsadaptor.cpp \
    ../../sensord/inputdevadaptor.cpp \
    ../../sensord/sockethandler.cpp \
    ../../sensord/propertyhandler.cpp \
    ../../sensord/calibrationhandler.cpp

INCLUDEPATH += ../.. \
    ../../include \
    ../../filters \
    ../../chains \
    ../../sensord \
    ../../datatypes \
    ../../chains \    
    ../../adaptors \
    ../../adaptors/sysfsadaptor 

QMAKE_LIBDIR_FLAGS += -L../../builddir/filters -L../../filters/ -lfilters
