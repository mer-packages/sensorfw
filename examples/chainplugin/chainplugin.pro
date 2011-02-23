TEMPLATE     = lib
CONFIG      += plugin

QT          += dbus

TARGET       = samplechain

include( ../../common-config.pri )

HEADERS += samplechain.h   \
           samplechainplugin.h

SOURCES += samplechain.cpp   \
           samplechainplugin.cpp

SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        ../../sensord \
                        ../../datatypes \
                        ../../filters \
                        ../../core \
                        ../../adaptors

DEPENDPATH  += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

include(../../common-install.pri)
publicheaders.files += $$HEADERS
target.path = $$PLUGINPATH
