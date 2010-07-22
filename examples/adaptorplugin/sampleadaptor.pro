TEMPLATE     = lib
CONFIG      += plugin

TARGET       = sampleadaptor

include(../../common-config.pri )

HEADERS += sampleadaptor.h \
           sampleadaptorplugin.h

SOURCES += sampleadaptor.cpp \
           sampleadaptorplugin.cpp

SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        ../../sensord \
                        ../../datatypes \
                        ../../filters

DEPENDPATH  += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

include(../../common-install.pri)
publicheaders.files += $$HEADERS
target.path = $$PLUGINPATH

INSTALLS += target
