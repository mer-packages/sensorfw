
include(../../common-config.pri)

TEMPLATE     = lib
CONFIG      += plugin

QT          += dbus

TARGET       = compasssensor

HEADERS += compasssensor.h \
           compasssensor_a.h \
           compassplugin.h

SOURCES += compasssensor.cpp \
           compasssensor_a.cpp \
           compassplugin.cpp

SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        ../../sensord \
                        ../../datatypes \
                        ../../filters \
                       
DEPENDPATH  += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

include(../../common-install.pri)
publicheaders.files += $$HEADERS
target.path = $$PLUGINPATH

INSTALLS += target
