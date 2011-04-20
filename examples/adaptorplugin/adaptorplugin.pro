TEMPLATE     = lib
CONFIG      += plugin

TARGET       = sampleadaptor

include(../../common-config.pri )

HEADERS += sampleadaptor.h \
           sampleadaptorplugin.h

SOURCES += sampleadaptor.cpp \
           sampleadaptorplugin.cpp

SENSORFW_INCLUDEPATHS = ../../include \
                        ../../sensord \
                        ../../datatypes \
                        ../../core \
                        ../../filters \
                        ../..

DEPENDPATH  += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS
