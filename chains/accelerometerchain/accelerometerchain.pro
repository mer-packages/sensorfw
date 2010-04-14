TEMPLATE     = lib
CONFIG      += plugin

QT          += dbus

TARGET       = accelerometerchain

include( ../../common-config.pri )

HEADERS += accelerometerchain.h   \
           accelerometerchainplugin.h

SOURCES += accelerometerchain.cpp   \
           accelerometerchainplugin.cpp

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
