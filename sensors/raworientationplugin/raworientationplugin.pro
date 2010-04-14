TEMPLATE     = lib
CONFIG      += plugin

QT          += dbus

TARGET       = raworientationsensor

include( ../../common-config.pri )

HEADERS += raworientationsensor.h   \
           raworientationsensor_a.h \
           raworientationplugin.h

SOURCES += raworientationsensor.cpp   \
           raworientationsensor_a.cpp \
           raworientationplugin.cpp

SENSORFW_INCLUDEPATHS = ../..           \
                        ../../include \
                        ../../sensord   \
                        ../../datatypes \
                        ../../filters
DEPENDPATH  += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

INCLUDEPATH += ../../libs

include(../../common-install.pri)

target.path = $$PLUGINPATH

INSTALLS += target
