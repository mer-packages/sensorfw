TEMPLATE     = lib
CONFIG      += plugin

QT          += dbus

TARGET       = samplesensor

include( ../../common-config.pri )

HEADERS += samplesensor.h \
           samplesensor_a.h \
           sampleplugin.h

SOURCES += samplesensor.cpp \
           samplesensor_a.cpp \
           sampleplugin.cpp

SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        ../../sensord \
                        ../../datatypes \
                        ../../core \
                        ../../filters

DEPENDPATH  += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

include(../../common-install.pri)
publicheaders.files += $$HEADERS
target.path = $$PLUGINPATH
