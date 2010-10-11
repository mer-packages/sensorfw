TEMPLATE     = lib
CONFIG      += plugin

TARGET       = alsadaptor-ascii

include( ../../common-config.pri )

HEADERS += alsadaptor-ascii.h \
           alsadaptor-asciiplugin.h

SOURCES += alsadaptor-ascii.cpp \
           alsadaptor-asciiplugin.cpp

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
