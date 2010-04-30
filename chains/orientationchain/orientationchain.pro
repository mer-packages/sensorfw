TEMPLATE     = lib
CONFIG      += plugin

QT          += dbus

TARGET       = orientationchain

include( ../../common-config.pri )

HEADERS += orientationchain.h   \
           orientationchainplugin.h

SOURCES += orientationchain.cpp   \
           orientationchainplugin.cpp

SENSORFW_INCLUDEPATHS = ../..           \
                        ../../include \
                        ../../sensord   \
                        ../../datatypes \
                        ../../filters \
                        ../../adaptors

DEPENDPATH  += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

include(../../common-install.pri)
publicheaders.files += $$HEADERS
target.path = $$PLUGINPATH

INSTALLS += target
