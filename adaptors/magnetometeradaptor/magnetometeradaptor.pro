TEMPLATE     = lib
CONFIG      += plugin

QT          += dbus

TARGET       = magnetometeradaptor

include( ../../common-config.pri )

HEADERS += magnetometeradaptor.h \
           magnetometeradaptorplugin.h

SOURCES += magnetometeradaptor.cpp \
           magnetometeradaptorplugin.cpp

SENSORFW_INCLUDEPATHS = ../.. \
                        ../../include \
                        ../ \
                        ../../sensord \
                        ../../datatypes \
                        ../../filters 

DEPENDPATH  += $$SENSORFW_INCLUDEPATHS
INCLUDEPATH += $$SENSORFW_INCLUDEPATHS

include(../../common-install.pri)
publicheaders.files += $$HEADERS
target.path = $$PLUGINPATH

INSTALLS += target
