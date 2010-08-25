TEMPLATE     = lib
CONFIG      += plugin

TARGET       = alsadaptor-sysfs

include( ../../common-config.pri )

HEADERS += alsadaptor-sysfs.h \
           alsadaptor-sysfsplugin.h

SOURCES += alsadaptor-sysfs.cpp \
           alsadaptor-sysfsplugin.cpp

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
