TEMPLATE     = lib
CONFIG      += plugin

TARGET       = samplechain

include( ../../common-config.pri )

HEADERS += samplechain.h   \
           samplechainplugin.h

SOURCES += samplechain.cpp   \
           samplechainplugin.cpp

SENSORFW_INCLUDEPATHS = ../../include \
                        ../../sensord \
                        ../../datatypes \
                        ../../filters \
                        ../../core \
                        ../../adaptors \
                        ../..

DEPENDPATH  += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS
