QT += dbus network

include(../common-install.pri)

TEMPLATE = app
TARGET = sensorfilters-test

CONFIG += testcase link_pkgconfig

PKGCONFIG += gconf-2.0

HEADERS += filtertests.h \
    ../../sensord/producer.h \
    ../../sensord/consumer.h \
    ../../sensord/bin.h \
    ../../sensord/ringbuffer.h \
    ../../sensord/filter.h \
    ../../sensord/source.h \
    ../../sensord/deviceadaptor.h \
    ../../sensord/abstractsensor.h \
    ../../sensord/sensormanager.h \
    ../../sensord/sensormanager_a.h \
    ../../sensord/loader.h \
    ../../sensord/plugin.h \
    ../../sensord/filterproperty.h \
    ../../sensord/parameterparser.h \
    ../../sensord/logging.h \
    ../../sensord/config.h \
    ../../sensord/sockethandler.h \
    ../../sensord/propertyhandler.h \
    ../../sensord/mcewatcher.h \
    ../../filters/orientationinterpreter/orientationinterpreter.h \
    ../../filters/coordinatealignfilter/coordinatealignfilter.h \
    ../../filters/declinationfilter/declinationfilter.h \
    ../../filters/rotationfilter/rotationfilter.h 

    
SOURCES += filtertests.cpp \
    ../../sensord/producer.cpp \
    ../../sensord/consumer.cpp \
    ../../sensord/bin.cpp \
    ../../sensord/ringbuffer.cpp \
    ../../sensord/filter.cpp \
    ../../sensord/source.cpp \
    ../../sensord/deviceadaptor.cpp \
    ../../sensord/abstractsensor.cpp \
    ../../sensord/sensormanager.cpp \
    ../../sensord/sensormanager_a.cpp \
    ../../sensord/loader.cpp \
    ../../sensord/plugin.cpp \
    ../../sensord/filterproperty.cpp \
    ../../sensord/parameterparser.cpp \
    ../../sensord/logging.cpp \
    ../../sensord/config.cpp \
    ../../sensord/sockethandler.cpp \
    ../../sensord/propertyhandler.cpp \
    ../../sensord/mcewatcher.cpp \
    ../../filters/orientationinterpreter/orientationinterpreter.cpp \
    ../../filters/coordinatealignfilter/coordinatealignfilter.cpp \
    ../../filters/declinationfilter/declinationfilter.cpp \
    ../../filters/rotationfilter/rotationfilter.cpp


    
INCLUDEPATH += ../.. \
    ../../include \
    ../../filters/ \
    ../../filters/orientationinterpreter \
    ../../filters/coordinatealignfilter \
    ../../filters/declinationfilter \
    ../../filters/rotationfilter \
    ../../sensord
    
LIBS += -L../../filters -lfilters \

