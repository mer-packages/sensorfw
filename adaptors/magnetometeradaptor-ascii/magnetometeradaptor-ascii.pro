TEMPLATE     = lib
CONFIG      += plugin

TARGET       = magnetometeradaptor-ascii

include( ../../common-config.pri )

HEADERS += magnetometeradaptor-ascii.h \
           magnetometeradaptor-asciiplugin.h

SOURCES += magnetometeradaptor-ascii.cpp \
           magnetometeradaptor-asciiplugin.cpp

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
